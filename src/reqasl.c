#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <fcntl.h> 

#include <errno.h> 
#include <locale.h>
#include <wchar.h> 

#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <libami.h>
#include "drawinfo.h"

#define MAX_CMD_CHARS 256
#define VISIBLE_CMD_CHARS 35

static const char ok_txt[]="Ok", vol_txt[]="Volumes", par_txt[]="Parent", cancel_txt[]="Cancel";
static const char drawer_txt[]="Drawer";
static const char file_txt[]="File";

XContext client_context, screen_context, icon_context;


gadget_button *buttons[6];
gadget_button b_ok, b_vol, b_par, b_cancel;
static const char *buttxt[]={ NULL, ok_txt, vol_txt, par_txt, cancel_txt };

// handle button choice and selection
static int selected=0, depressed=0;

// handling input fields
static int stractive_dir=0, selected_dir=0, depressed_dir=0;
static int selected_file=0, stractive_file=0, depressed_file=0;
// char dir_path[MAX_CMD_CHARS+1];
// char file_path[MAX_CMD_CHARS+1];
char *dir_path; // path displayed in directory input field
int buf_len_dir=0;
int buf_len_file=0;
int cur_pos_dir=0;
int cur_pos_file=0;
int left_pos_dir=0;
int left_pos_file=0;
int cur_x=6;

char *progname;
Display *dpy;
struct DrawInfo dri;

Window root, mainwin, IFdir, IFfile, List, input; // input is pointing to active input field
int win_x=20, win_y=20, win_width=262, win_height=250; // main window dimensions
int list_width = 255, list_height = 155;  // file listing window dimensions
int IFdir_width=0, IFdir_height=20;       // input field for directories
int IFfile_width=0, IFfile_height=20;     // input field for files

GC gc;

Atom amiwm_menu = None, wm_protocols = None, wm_delete = None;

/** width and height of input field borders */
int strgadw, strgadh;

/** font height */
int fh;

/** window pos x and y */
int mainx=100, mainy=20;

/** window width and height */
int mainw, mainh;

/** button width */
int butw;

/** keep scroll offset, cleared when entering a new path */
int offset_y=0;

/** store screen resolution */
int screen_width=0;
int screen_height=0;

// store browsing progression
char *current_dir="";
char *parent_dir="";

static XIM xim = (XIM) NULL;
static XIC xic = (XIC) NULL;

void refresh_IFborders(Window win);


struct fs_entity
{
  char *name;
  char *path;
  char *type;   // file or dir
  int x;
  int y;
  int width;    // active dimensions
  int height;
  Window win;
  Pixmap pm1;
  Pixmap pm2;
  Pixmap pmA;
  Bool selected;
  Bool dragging;
}; typedef struct fs_entity fs_entity;

fs_entity *fse_arr; // array for normal files 
int alloc_ecount = 20; // array size 
int entries_count; // total amount of entries per dir 

void clean_reset()
{
  for (int i=0;i<entries_count;i++)
  {
    fse_arr[i].name = None;
    fse_arr[i].path = None;
    fse_arr[i].type = None;
    fse_arr[i].pm1 = None;
    fse_arr[i].pm2 = None;
    fse_arr[i].pmA = None;
    fse_arr[i].win = None;
  }
  XDestroySubwindows(dpy,List); 
  XClearWindow(dpy,List);
  
  entries_count = 0; // clear count of files
  alloc_ecount = 0;  // clear struct allocation count
  offset_y = 0;      // clear scroll offset
  free(fse_arr);
  
  // restart fresh allocation of struct in array
  alloc_ecount = 20;
  fse_arr = calloc(alloc_ecount, sizeof(fs_entity)); 
}

static void SetMenu(Window w)
{
  if (amiwm_menu == None)
    amiwm_menu = XInternAtom(dpy, "AMIWM_MENU", False);
  XChangeProperty(dpy, w, amiwm_menu, amiwm_menu, 8, PropModeReplace,
                  "M0Main\0S00Color\0K0RRed\0K0GGreen\0K0BBlue\0EK0XExit\0"
                  "M0Help\0I0About", 65);
}

static void SetProtocols(Window w)
{
  if (wm_protocols == None)
    wm_protocols = XInternAtom(dpy, "WM_PROTOCOLS", False);
  if (wm_delete == None)
    wm_delete = XInternAtom(dpy, "WM_DELETE_WINDOW", False);
  XChangeProperty(dpy, w, wm_protocols, XA_ATOM, 32, PropModeReplace,
                  (char *)&wm_delete, 1);
}

int function_name(const void *arg1, const void *arg2)
{
  const fs_entity *entity1 = arg1; const fs_entity *entity2 = arg2;
  if(strcmp(entity1->type,entity2->type)==0)
    return strcmp(entity1->name, entity2->name );

  if(strcmp(entity1->type,"directory")==0)
    return -1;
  else // if(strcmp(entity2->type,"directory")==0)
    return 1;
}


void getlabels(char *path)
{ 
  DIR *dirp;
  struct dirent *dp;
  int file_count=0; 

  if((dirp = opendir(path)) == NULL) 
  {
    fprintf(stderr,"cannot open directory: %s\n", path);
    return;
  }
  while ((dp = readdir(dirp)) != NULL)
  {
    if (entries_count==alloc_ecount)
    {
      alloc_ecount+=(alloc_ecount/2);
      fse_arr = realloc(fse_arr,alloc_ecount * sizeof(fs_entity));
      printf("new alloc_ecount=%d\n",alloc_ecount);
    }
    if (dp->d_type & DT_DIR)
    {
      if (dp->d_name[0] != '.')
      {
        fse_arr[entries_count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(fse_arr[entries_count].name, dp->d_name);
        int pathsize = strlen(path) + strlen(fse_arr[entries_count].name) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        strcat(tempo,fse_arr[entries_count].name);
        strcat(tempo,"/");
        fse_arr[entries_count].path = tempo;
        //printf("directory, path=%s name=%s\n",entries[count].path,entries[count].name);
        fse_arr[entries_count].type = "directory";
        entries_count++;
        file_count++;
      }
    }

    else if (dp->d_type & DT_REG)
    {
      if (dp->d_name[0] != '.')
      {
        //printf ("FILE: %s\n", dp->d_name);
        //wbicon_data(count, dp->d_name, path, "file" );
        fse_arr[entries_count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(fse_arr[entries_count].name, dp->d_name);
        int pathsize = strlen(path) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        fse_arr[entries_count].path = tempo;
        //printf("file, path=%s name=%s\n",entries[count].path,entries[count].name);
        fse_arr[entries_count].type = "file";
        entries_count++;
        file_count++;
      }
    }
  }
  closedir(dirp);
  printf("file_count=%d\n",file_count);

  if (fse_arr[0].name == NULL)
  {
    // save current_dir now in case we enter an empty dir with 
    // nothing to work on to set previous/current pathes
    char *buf="";
    buf=strdup(path);
    buf[strlen(buf)-1] ='\0';
    
    char * ptr;
    int    ch = '/';
    ptr = strrchr( buf, ch ); // find where next "last" slash is now
    int pos = (ptr-buf);
    //printf("pos=%d\n",pos);
    
    if (ptr !=NULL && pos !=0)
    {
      
      char *newbuff = malloc(pos);
      memcpy(newbuff,buf,pos);  // sample: "/home/klaus/Downloads"
      newbuff[pos] = '/';
      newbuff[pos+1]='\0';
      printf("(E) current_path=%s\n",path);
      printf("(E) parent path=%s\n",newbuff);
      

      current_dir=path;
      parent_dir = newbuff;
    }
  }
  if (fse_arr[0].name != NULL)
  {
    char *buf="";
    if(strcmp(fse_arr[0].type, "file")==0)
    {
      current_dir=fse_arr[0].path;
      buf=fse_arr[0].path;  //sample: "/home/klaus/Downlads/icons/"
      printf("(f) current path=%s\n",buf);
      buf[strlen(buf)-1] ='\0'; //remove final slash

      char * ptr;
      int    ch = '/';
      ptr = strrchr( buf, ch ); // find where next "last" slash is now
      int pos = (ptr-buf);

      if (ptr !=NULL)
      {
        char *newbuff = malloc(pos);
        memcpy(newbuff,buf,pos);  // sample: "/home/klaus/Downloads"
        newbuff[pos] = '/';
        newbuff[pos+1] = '\0';
        printf("(f) parent path=%s \n\n",newbuff);
        parent_dir = newbuff;
      }
    }

    else if(strcmp(fse_arr[0].type, "directory")==0)
    {
      buf=fse_arr[0].path;
      //printf("DIR_ORIG: buf_len=%lu buf=%s\n",strlen(buf),buf);
      buf[strlen(buf)-1] ='\0';

      char * ptr;
      int    ch = '/';
      ptr = strrchr( buf, ch ); // find where next "last" slash is now
      int pos = (ptr-buf);
      //printf("pos=%d\n",pos);

      if (ptr !=NULL && pos !=0)
      {

        char *newbuff = malloc(pos);
        memcpy(newbuff,buf,pos);  // sample: "/home/klaus/Downloads"
        newbuff[pos] = '\0';
        printf("(d) current path=%s\n",newbuff);
        //printf("DIR_SEMI_FINAL: len_of_newbuff=%lu newbuff=%s \n",strlen(newbuff),newbuff);

        char *nptr;
        int ch = '/';
        nptr = strrchr( newbuff, ch );
        pos = (nptr-newbuff);
        //printf("npos=%d\n",pos);
        char *dirbuff = malloc(pos);
        memcpy(dirbuff,newbuff,pos);
        dirbuff[pos] = '/';
        dirbuff[pos+1] = '\0';

        printf("(d) parent path=%s \n\n",dirbuff);
        parent_dir = dirbuff;

      }
    }
  }
  qsort(fse_arr, entries_count, sizeof(fs_entity), function_name);
}

void list_entries()
{
  //viewmode="list";
  //printf("VIEWMODE now =%s\n",get_viewmode()); 
  for (int i=0;i<entries_count;i++)
  {
    //printf("list_entries count i=%d\n",i);
    fse_arr[i].width  = win_width-50; //list_width;
    fse_arr[i].height = win_height-100;
    fse_arr[i].x = 0;
    fse_arr[i].y = 5 + i*16;
    fse_arr[i].y += offset_y;
    XMoveWindow(dpy,fse_arr[i].win,fse_arr[i].x,fse_arr[i].y );
  }
}

void build_entries()
{
  XSetWindowAttributes xswa;
  for (int i=0;i<entries_count;i++)
  { 
    //int width = XmbTextEscapement(dri.dri_FontSet, entries[i].name, strlen(entries[i].name));
    fse_arr[i].width  = list_width;
    fse_arr[i].height = 15;

//     fse_arr[i].win=XCreateSimpleWindow(dpy, List, fse_arr[i].x, fse_arr[i].y, 
//                                        fse_arr[i].width, fse_arr[i].height, 0,
//                                        dri.dri_Pens[SHADOWPEN],
//                                        dri.dri_Pens[BACKGROUNDPEN]);
    
    fse_arr[i].win = XCreateWindow( dpy,List, fse_arr[i].x, fse_arr[i].y, fse_arr[i].width, fse_arr[i].height, 0, 24, InputOutput, CopyFromParent, CWBackPixel, &xswa);

    fse_arr[i].pm1 = XCreatePixmap(dpy, fse_arr[i].win, fse_arr[i].width, fse_arr[i].height, 24);
    fse_arr[i].pm2 = XCreatePixmap(dpy, fse_arr[i].win, fse_arr[i].width, fse_arr[i].height, 24);

    XSetForeground(dpy, gc, 0xaaaaaa);
    XFillRectangle(dpy, fse_arr[i].pm1, gc, 0, 0, fse_arr[i].width, fse_arr[i].height);

    if (strcmp(fse_arr[i].type,"directory")==0)
    {
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
      XFillRectangle(dpy, fse_arr[i].pm1, gc, 0, 0, fse_arr[i].width, fse_arr[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, fse_arr[i].pm1, gc, win_width-75, 12, "Drawer", strlen("Drawer"));
      XDrawImageString(dpy, fse_arr[i].pm1, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
      XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);

      XSetBackground(dpy, gc, dri.dri_Pens[FILLPEN]);
      XFillRectangle(dpy, fse_arr[i].pm2, gc, 0, 0, fse_arr[i].width, fse_arr[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, fse_arr[i].pm2, gc, win_width-75, 12, "Drawer", strlen("Drawer"));
      XDrawImageString(dpy, fse_arr[i].pm2, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
    }
    else if (strcmp(fse_arr[i].type,"file")==0)
    {
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
      XFillRectangle(dpy, fse_arr[i].pm1, gc, 0, 0, fse_arr[i].width, fse_arr[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, fse_arr[i].pm1, gc, win_width-65, 12, "---", strlen("---"));
      XDrawImageString(dpy, fse_arr[i].pm1, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
      XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);

      XSetBackground(dpy, gc, dri.dri_Pens[FILLPEN]);
      XFillRectangle(dpy, fse_arr[i].pm2, gc, 0, 0, fse_arr[i].width, fse_arr[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, fse_arr[i].pm2, gc, win_width-65, 12, "---", strlen("---"));
      XDrawImageString(dpy, fse_arr[i].pm2, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
    }
    XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    fse_arr[i].pmA = fse_arr[i].pm1; // set active pixmap
    XSetWindowBackgroundPixmap(dpy, fse_arr[i].win, fse_arr[i].pmA);

    XMapWindow(dpy,fse_arr[i].win);
    XSelectInput(dpy, fse_arr[i].win, ExposureMask|CWOverrideRedirect|KeyPressMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|ShiftMask);
  }
}

/** get button number/id */
int getchoice(Window w)
{
  
  if (b_ok.w == w)
    return 1;
  else if (b_vol.w == w)
    return 2;
  else if (b_par.w == w)
    return 3;
  else if (b_cancel.w == w)
    return 4;
  else
    return 0;
//   int i;
//   int totalbuttons = (int)(sizeof button / sizeof button[0]);
//   for(i=1; i<totalbuttons; i++)
//   {
//     if(button[i]==w)
//     {
//       printf("button id=%d\n",i);
//       return i;
//     }
//   }
//   return 0;
  
//   int i;
//   for (i=1; i<3; i++) 
//   {
//     if (buttons[i]->w == w)
//       return i;
//   }
     //return 0;
}

// refresh buttons in window 
// void refresh_button(Window w, const char *txt, int idx)
// {
//   printf("refresh_button \n");
//   int h=25;
//   int l=strlen(txt);
//   int tw=XmbTextEscapement(dri.dri_FontSet, txt, l);
//   XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
//   XDrawString(dpy, w, gc, (butw-tw)>>1, dri.dri_Ascent+BUT_VSPACE, txt, l);
//   XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)? SHADOWPEN:SHINEPEN]);
//   XDrawLine(dpy, w, gc, 0, 0, butw-2, 0);
//   XDrawLine(dpy, w, gc, 0, 0, 0, h-2);
//   XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)? SHINEPEN:SHADOWPEN]);
//   XDrawLine(dpy, w, gc, 1, h-1, butw-1, h-1);
//   XDrawLine(dpy, w, gc, butw-1, 1, butw-1, h-1);
//   XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
//   XDrawPoint(dpy, w, gc, butw-1, 0);
//   XDrawPoint(dpy, w, gc, 0, h-1);
//   printf("refresh_idx=%d\n",idx);
// }

/** refresh window background */
static void refresh_main(void)
{
  int w;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  w=XmbTextEscapement(dri.dri_FontSet, drawer_txt, strlen(drawer_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 10, win_height-60, drawer_txt, strlen(drawer_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, win_height-36, file_txt, strlen(file_txt));
}


/** refresh text string in input field (typing) */
extern size_t mbrlen(); // define proto mbrlen() to silence ccompile warning
void refresh_str_text_dir()
{

  int string_length=XmbTextEscapement(dri.dri_FontSet, current_dir, strlen(current_dir)); 
  int offset_x = string_length - IFdir_width;
  
  //clean
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, IFdir, gc, 0, 3,IFdir_width, fh);
  
  //write    
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(string_length>IFdir_width)
  {
    
    XmbDrawImageString(dpy, IFdir, dri.dri_FontSet,
                      gc, -(offset_x +20 ), 3+dri.dri_Ascent,
                       current_dir, strlen(current_dir));
  } 
  else 
  {
    XmbDrawImageString(dpy, IFdir, dri.dri_FontSet,
                        gc, 6, 3+dri.dri_Ascent,
                        current_dir, strlen(current_dir)); 

  }
  
  // if input field active show carret
  if (stractive_dir)
  {
    XSetForeground(dpy, gc, ~0);
    XFillRectangle(dpy, IFdir, gc, cur_pos_dir, 3, 8, fh);
  }
  refresh_IFborders(IFdir);
  
  /*
  XSetForeground(dpy, gc, ~0);
  XFillRectangle(dpy, input, gc, string_length, 3,
                 XExtentsOfFontSet(dri.dri_FontSet)->
                 max_logical_extent.width, fh);
  */
  
  
  /*
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len_dir>left_pos_dir) 
  {
    int w, c;
    for(l=0; l<buf_len_dir-left_pos_dir; ) {
      c=mbrlen(dir_path+left_pos_dir+l, buf_len_dir-left_pos_dir-l, NULL);
      w=6+XmbTextEscapement(dri.dri_FontSet, dir_path+left_pos_dir, l+c);
      if(w>strgadw-6)
        break;
      mx=w;
      l+=c;
    }

    XmbDrawImageString(dpy, input, dri.dri_FontSet,
                       gc, 6, 3+dri.dri_Ascent,
                       dir_path+left_pos_dir, l);
  }
  
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, input, gc, mx, 3, strgadw-mx-6, fh);
  if(stractive_dir) 
  {
    if(cur_pos_dir<buf_len_dir) {
      XSetBackground(dpy, gc, ~0);

      l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
      XmbDrawImageString(dpy, input, dri.dri_FontSet,
                         gc, cur_x, 3+dri.dri_Ascent,
                         dir_path+cur_pos_dir, l);
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    }
    
    else 
    {
      XSetForeground(dpy, gc, ~0);
      XFillRectangle(dpy, input, gc, cur_x, 3,
                     XExtentsOfFontSet(dri.dri_FontSet)->
                     max_logical_extent.width, fh);
    }
  }
*/
}


/** work with text string entered */
void strkey_dir(XKeyEvent *e)
{
  //void endchoice(void);
  Status stat;
  KeySym ks;
  char *buf_dir=current_dir;

  
  int x, i, n;
  
  n=XmbLookupString(xic, e, buf_dir, sizeof(buf_dir), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)
    
    switch(ks) 
    {
      case XK_Return:
      case XK_Linefeed:
        selected=1;
        //endchoice();
        break;
      case XK_Left:
        
        break;
      case XK_Right:
        if(cur_pos_dir<buf_len_dir) 
        {
          int l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
          if(l>0)
            cur_pos_dir+=l;
        }
        break;
      default:
        if(stat == XLookupBoth)
          stat = XLookupChars;
    }
//   int string_length=XmbTextEscapement(dri.dri_FontSet, buf_dir, strlen(buf_dir)); 
//   XSetForeground(dpy, gc, ~0);
//   XFillRectangle(dpy, IFdir, gc, string_length, 3, 4, fh);
  refresh_str_text_dir();
  
  /*
  printf("cur_x=%d  cur_pos_dir%d\n",cur_x, cur_pos_dir);
  void endchoice(void);
  Status stat;
  KeySym ks;

  //char buf[256];

  char buf_dir[256];
  for(int i=0;i<strlen(current_dir);i++)
  {
    buf_dir[i] = current_dir[i];
    
  }  
  printf("buf_dir=%s current_dir=%s\n",buf_dir,current_dir);

  int x, i, n;
  n=XmbLookupString(xic, e, buf_dir, sizeof(buf_dir), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)

    switch(ks) {
      case XK_Return:
      case XK_Linefeed:
        selected=1;
        endchoice();
        break;
      case XK_Left:
        if(cur_pos_dir) {
          int p=cur_pos_dir;
          while(p>0) {
            --p;
            if(((int)mbrlen(dir_path+p, cur_pos_dir-p, NULL))>0) {
              cur_pos_dir=p;
              break;
            }
          }
        }
        break;
      case XK_Right:
        if(cur_pos_dir<buf_len_dir) {
          int l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
          if(l>0)
            cur_pos_dir+=l;
        }
        break;
      case XK_Begin:
        cur_pos_dir=0;
        break;
      case XK_End:
        cur_pos_dir=buf_len_dir;
        break;
      case XK_Delete:
        if(cur_pos_dir<buf_len_dir) {
          int l=1;
          l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
          if(l<=0)
            break;
          buf_len_dir-=l;
          for(x=cur_pos_dir; x<buf_len_dir; x++){
            dir_path[x]=dir_path[x+l];
          }
          dir_path[x] = 0;
        }
        else {
          XBell(dpy, 100);
        }
        break;
      case XK_BackSpace:
        if(cur_pos_dir>0) {
          int l=1;
          int p=cur_pos_dir;
          while(p>0) {
            --p;
            if(((int)mbrlen(dir_path+p, cur_pos_dir-p, NULL))>0) {
              l=cur_pos_dir-p;
              break;
            }
          }
          buf_len_dir-=l;
          for(x=(cur_pos_dir-=l); x<buf_len_dir; x++) {
            dir_path[x]=dir_path[x+l];
          }
          dir_path[x] = 0;
        }
        else {
          XBell(dpy, 100);
        }
        break;
      default:
        if(stat == XLookupBoth)
          stat = XLookupChars;
    }
  if(stat == XLookupChars) {
    for(i=0; i<n && buf_len_dir<MAX_CMD_CHARS; i++) {
      for(x=buf_len_dir; x>cur_pos_dir; --x)
        dir_path[x]=dir_path[x-1];
      dir_path[cur_pos_dir++]=buf_dir[i];
      buf_len_dir++;
    }
    if(i<n)
      XBell(dpy, 100);
  }
  if(cur_pos_dir<left_pos_dir)
    left_pos_dir=cur_pos_dir;
  cur_x=6;

  if(cur_pos_dir>left_pos_dir)
    cur_x+=XmbTextEscapement(dri.dri_FontSet, dir_path+left_pos_dir, cur_pos_dir-left_pos_dir);
  if(cur_pos_dir<buf_len_dir) {
    int l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
    x=XmbTextEscapement(dri.dri_FontSet, dir_path+cur_pos_dir, l);
  } else
    x=XExtentsOfFontSet(dri.dri_FontSet)->max_logical_extent.width;

  if((x+=cur_x-(strgadw-6))>0) {
    cur_x-=x;
    while(x>0) {
      int l=mbrlen(dir_path+left_pos_dir, buf_len_dir-left_pos_dir, NULL);
      x-=XmbTextEscapement(dri.dri_FontSet, dir_path+left_pos_dir, l);
      left_pos_dir += l;
    }
    cur_x+=x;
  }
  refresh_str_text_dir();
  */
}

/** click on input field (stractive) */
void strbutton_dir(XButtonEvent *e)
{
  printf("entering strbutton_dir\n");
  stractive_dir=1;
  
  printf("e.x=%d\n",e->x);
  refresh_str_text_dir(); 
}

void refresh_str_text_file() { refresh_IFborders(IFfile); }
/** work with text string entered */
void strkey_file(XKeyEvent *e) { }

void refresh_list()
{
  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, mainwin, gc, 9, 9, win_width-10, 9);
  XDrawLine(dpy, mainwin, gc, 9, 9, 9, win_height-85);
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  XDrawLine(dpy, mainwin, gc, 9, win_height-85, win_width-10,win_height-85);
  XDrawLine(dpy, mainwin, gc, win_width-10, win_height-85, win_width-10, 9);
}

/** refresh drawing of text field borders */
void refresh_IFborders(Window win)
{
  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, win, gc, 0, strgadh-1, 0, 0);
  XDrawLine(dpy, win, gc, 0, 0, strgadw-2, 0);
  XDrawLine(dpy, win, gc, 3, strgadh-2, strgadw-4, strgadh-2);
  XDrawLine(dpy, win, gc, strgadw-4, strgadh-2, strgadw-4, 2);
  XDrawLine(dpy, win, gc, 1, 1, 1, strgadh-2);
  XDrawLine(dpy, win, gc, strgadw-3, 1, strgadw-3, strgadh-2);
  XSetForeground(dpy, gc, dri.dri_Pens[SHADOWPEN]);
  XDrawLine(dpy, win, gc, 1, strgadh-1, strgadw-1, strgadh-1);
  XDrawLine(dpy, win, gc, strgadw-1, strgadh-1, strgadw-1, 0);
  XDrawLine(dpy, win, gc, 3, strgadh-3, 3, 1);
  XDrawLine(dpy, win, gc, 3, 1, strgadw-4, 1);
  XDrawLine(dpy, win, gc, 2, 1, 2, strgadh-2);
  XDrawLine(dpy, win, gc, strgadw-2, 1, strgadw-2, strgadh-2);
}


/** click on input field (stractive) */
void strbutton_file(XButtonEvent *e)
{
}

void set_cursor_pos(int x_pos)
{
  if (x_pos == -1) {
    // initial position
    int string_length=XmbTextEscapement(dri.dri_FontSet, current_dir, strlen(current_dir)); 
    if(string_length>IFdir_width)
    {
      cur_pos_dir = (IFdir_width-20);
    }
    else 
    {
      cur_pos_dir = (string_length+8);
    }
  } 
  // position choosen with mouse or keyboard
  else 
  {
    cur_pos_dir = x_pos;
  }
}

static void toggle(int c)
{
//   XSetWindowBackground(dpy, button[c],
//                        dri.dri_Pens[(depressed&&c==selected)?
//                        FILLPEN:BACKGROUNDPEN]);
//   XClearWindow(dpy, button[c]);
//   refresh_button(button[c], buttxt[c], c);
  //printf("toggle = %d\n\n",c);
  if (c == 0)
    return;
  //gadget_button_toggle(buttons[c]);
}

//void toggle_choice(int c)
//{
//   XSetWindowBackground(dpy, button[c],
//                        dri.dri_Pens[(depressed&&c==selected)?
//                        FILLPEN:BACKGROUNDPEN]);
//   XClearWindow(dpy, button[c]);
//   refresh_button(button[c], buttxt[c], c);
//}

static void abortchoice()
{
//   if(depressed) {
//     depressed=0;
//     toggle(selected);
//   }
//   selected=0;
  if(depressed) {
    b_ok.depressed=0;
    if (selected > 0) {
      //gadget_button_set_depressed(buttons[selected], 0);
    }
    button_toggle(&b_ok);
  }
  selected = 0;
}

void got_file(char *path, char *name)
{
  const char *cmd = "mimeopen";
  //const char *path = fse_arr[i].path;
  //const char *exec = fse_arr[i].name;
  strcat(path,"/");
  char *line=alloca(strlen(cmd) + strlen(path) + strlen(name) +2);
  sprintf(line, "%s %s%s &", cmd, path, name);
  system(line);
}

void got_path(char *path)
{
  
  //TODO: check if selection is file(s) then if yes mimeopen them 
  printf("received path from input filed=%s\n", path);
  if( access( path, F_OK ) == 0 ) { // check if path exists
    int length = strlen(path);
    if (path[length-1] != '/') //check if final slash exists
    {
      //printf("ALARM\n");
      char *buf = malloc(length+1);
      memcpy(buf,path,length);
      buf[length]='/';
      buf[length+1]='\0';
      //printf("added final slash, buf is now=%s\n",buf);
      memcpy(path,buf,length+1);
      //printf("overwrote, path is now=%s\n",buf);
    }
    
  clean_reset();
  getlabels(path);
  build_entries();
  list_entries();
  current_dir=path;
  set_cursor_pos(-1); // set cursor to inital position
  refresh_str_text_dir();
  refresh_str_text_file();
  }
  else {printf("input path do not exists\n\n");}
}

void endchoice(int c)
{
//   int c=selected;
//   abortchoice();

  if(c==1){
    //     printf("ss_cmdline=%s\n", cmdline);
    //     system(cmdline);

    if (input == IFdir)
      got_path(dir_path);
//     else if(input == IFfile)
//       got_path(file_path);
  }
  if(c==2){
    printf("volumes\n");
    current_dir="/";
    parent_dir="/";
    printf("(v) current path=/\n");
    printf("(v) parent path=/\n\n");
    got_path("/");
  }
  if(c==3){
    printf("parent\n");
    if(parent_dir != NULL)
    {
      if (strcmp(parent_dir,"/")==0) {
        printf("(p) current dir=/\n");
        printf("(p) parent dir=/\n\n");
      } 
      got_path(parent_dir);
    }
  }
  if (c==4){
    printf("gracefully end\n");
    XCloseDisplay(dpy);
    exit(0);
  }
}

int button_spread(int pos, int div)
{
  int result;
  if (pos*(win_width/div) > pos*60){
    result = pos*(win_width/div);
    return result;
  }
  else {
    return pos*60;
  }
}


int main(int argc, char *argv[])
{
  
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  // clickable buttons
  //Window b_ok, b_vol, b_par, b_cancel;
  int w2, c;
  char *p;
  //setlocale(LC_CTYPE, "");
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
            XDisplayName(NULL));
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  int snum = DefaultScreen(dpy);
  screen_width = DisplayWidth(dpy, snum);
  screen_height = DisplayHeight(dpy, snum);

  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  int s_middle_x = (screen_width/2) - win_width/2;
  int s_middle_y = (screen_height/2) - win_height;
  
  strgadw=win_width-70;
  strgadh=(fh=dri.dri_Ascent+dri.dri_Descent)+6;

  mainwin=XCreateSimpleWindow(dpy, root, s_middle_x, s_middle_y, win_width, win_height, 0,
                              dri.dri_Pens[SHADOWPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);
  //win_width-20, win_height-95
  List=XCreateSimpleWindow(dpy, mainwin, 10, 10, list_width, list_height, 0,
                           dri.dri_Pens[SHADOWPEN],
                           dri.dri_Pens[BACKGROUNDPEN]);

  IFdir=XCreateSimpleWindow(dpy, mainwin, 70, win_height-73, win_width-70, 20, 0,
                            dri.dri_Pens[SHADOWPEN],
                            dri.dri_Pens[BACKGROUNDPEN]);

  IFfile=XCreateSimpleWindow(dpy, mainwin, 70, win_height-50, win_width-70, 20, 0,
                             dri.dri_Pens[SHADOWPEN],
                             dri.dri_Pens[BACKGROUNDPEN]);

  

  
//   b_ok=XCreateSimpleWindow(dpy, mainwin,0, win_height-25, 50, 25, 0,
//                            dri.dri_Pens[SHADOWPEN],
//                            dri.dri_Pens[BACKGROUNDPEN]);
// 
//   b_vol=XCreateSimpleWindow(dpy, mainwin, 50, win_height-30, 50, 25, 0,
//                             dri.dri_Pens[SHADOWPEN],
//                             dri.dri_Pens[BACKGROUNDPEN]);
// 
//   b_par=XCreateSimpleWindow(dpy, mainwin, 100, win_height-30, 50, 25, 0,
//                             dri.dri_Pens[SHADOWPEN],
//                             dri.dri_Pens[BACKGROUNDPEN]);
// 
//   b_cancel=XCreateSimpleWindow(dpy, mainwin, 150, win_height-25, 50, 25, 0,
//                                dri.dri_Pens[SHADOWPEN],
//                                dri.dri_Pens[BACKGROUNDPEN]);
  
//   buttons[0]=None;
//   buttons[1]=b_ok;
//   buttons[2]=b_vol;
//   buttons[3]=b_par;
//   buttons[4]=b_cancel;

  //totalbuttons = sizeof button / sizeof button[0];
  //printf("totalbuttonlength=%d\n",(int)(sizeof button / sizeof button[0]));
  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, List, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, IFdir, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, IFfile, ExposureMask|StructureNotifyMask|ButtonPressMask);
  //XSelectInput(dpy, b_ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  //XSelectInput(dpy, b_vol, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  //XSelectInput(dpy, b_par, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  //XSelectInput(dpy, b_cancel, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  
  
  b_ok = *button_create(dpy, &dri, gc, mainwin, 0, win_height-35 ); 
  button_set_text(&b_ok, ok_txt);
  
  b_vol = *button_create(dpy, &dri, gc, mainwin, 80, win_height-35 ); 
  button_set_text(&b_vol, vol_txt);
  
  b_par = *button_create(dpy, &dri, gc, mainwin, 160, win_height-35 ); 
  button_set_text(&b_par, par_txt);
  
  b_cancel = *button_create(dpy, &dri, gc, mainwin, 240, win_height-35 ); 
  button_set_text(&b_cancel, cancel_txt);
  
  
  XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);

  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  
/*
  if ((p = XSetLocaleModifiers("@im=none")) != NULL && *p)
    xim = XOpenIM(dpy, NULL, NULL, NULL);
  if (!xim)
    fprintf(stderr, "Failed to open input method.\n");
  else {
    xic = XCreateIC(xim,
                    XNInputStyle, XIMPreeditNothing | XIMStatusNothing,
                    XNClientWindow, mainwin,
                    XNFocusWindow, mainwin,
                    NULL);
    if (!xic)
      fprintf(stderr, "Failed to create input context.\n");
  }
  if (!xic)
    exit(1);
*/
  
  SetProtocols(mainwin);
  SetMenu(mainwin);
  
  size_hints.flags = PMinSize|PMaxSize; //PResizeInc;
  //hints->flags = PMinSize|PMaxSize;
  size_hints.min_width = 255;
  size_hints.max_width = 700;
  size_hints.min_height = 250;
  size_hints.max_height = 500;
  XSetWMNormalHints(dpy, mainwin, &size_hints);
  XSetWMSizeHints(dpy, mainwin, &size_hints, PMinSize|PMaxSize);
  txtprop1.value=(unsigned char *)"Select";
  txtprop2.value=(unsigned char *)"ExecuteCmd";
  txtprop2.encoding=txtprop1.encoding=XA_STRING;
  txtprop2.format=txtprop1.format=8;
  txtprop1.nitems=strlen((char *)txtprop1.value);
  txtprop2.nitems=strlen((char *)txtprop2.value);
  XSetWMProperties(dpy, mainwin,
                   &txtprop1,
                   &txtprop2, argv, argc,
                   &size_hints, NULL, NULL);

  char homedir[50];
  snprintf(homedir, sizeof(homedir) , "%s", getenv("HOME"));

  // add final slash,  getenv("HOME") provides
  int pos = strlen(homedir);
  char *newbuff = malloc(pos);

  memcpy(newbuff,homedir,pos);  // sample: "/home/klaus/Downloads"
  newbuff[pos] = '/';
  newbuff[pos+1] = '\0';
  strcpy(homedir,newbuff);
  free(newbuff);

  XClassHint *myhint = XAllocClassHint();
  myhint->res_class="reqasl";
  myhint->res_name="ReqASL";
  XSetClassHint(dpy,mainwin,myhint);
  
  current_dir = homedir;
  got_path(homedir); // build files listing
  //got_path(homedir); // two time's the charm
  


  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    //#ifdef USE_FONTSETS
//     if(!XFilterEvent(&event, mainwin)) 
//     {
      //#endif
      switch(event.type) 
      {
        case Expose:
//           if(!event.xexpose.count) 
//           {
          if(event.xexpose.window == mainwin) 
          {
            refresh_main();
          }
          if(event.xexpose.window == List) 
          {
            refresh_str_text_dir(); // inputfield was covered, refresh it
            refresh_list();
          }
          if(event.xexpose.window == IFdir) 
          {
            input=IFdir;
            refresh_IFborders(IFdir);
          }
          if(event.xexpose.window == IFfile) 
          {
            input=IFfile;
            refresh_IFborders(IFfile);
          }
          if(event.xexpose.window == b_ok.w) 
          {
            button_refresh(&b_ok);
          }
          if(event.xexpose.window == b_vol.w) 
          {
            //refresh_button(b_vol, vol_txt, 2);
            button_refresh(&b_vol);
          }
          if(event.xexpose.window == b_par.w) 
          {
            //refresh_button(b_par, par_txt, 3);
            button_refresh(&b_par);
          }
          if(event.xexpose.window == b_cancel.w) 
          {
            //refresh_button(b_cancel, cancel_txt, 4);
            button_refresh(&b_cancel);
          }

        //}
      case ConfigureNotify:
        if(event.xconfigure.window == mainwin) 
        {
          // make save and cancel button to stay at bottom of window when it gets resized
          win_x=event.xconfigure.x;
          win_y=event.xconfigure.y;
          if (event.xconfigure.width < 255) 
          {
            win_width=255;
          }
          else 
          {
            win_width=event.xconfigure.width;
          }
          win_height=event.xconfigure.height;
          strgadw=win_width-70;
          list_width = win_width-20;
          list_height = win_height-95;
          IFdir_width = win_width-70;
          IFfile_width = win_width-70;
          XMoveWindow(dpy, IFdir, 60, win_height-73);
          XMoveWindow(dpy, IFfile, 60, win_height-50);
          XMoveWindow(dpy, b_ok.w, button_spread(0,4)+5, event.xconfigure.height - 22);       // 10
          XMoveWindow(dpy, b_vol.w, button_spread(1,4)+5, event.xconfigure.height - 22);      // 70
          XMoveWindow(dpy, b_par.w, button_spread(2,4)+5, event.xconfigure.height - 22);     // 130
          XMoveWindow(dpy, b_cancel.w,button_spread(3,4)+5 , event.xconfigure.height - 22); // 190
          XResizeWindow(dpy,List,list_width, list_height);
          XResizeWindow(dpy,IFdir,IFdir_width, IFdir_height);
          XResizeWindow(dpy,IFfile,IFfile_width, IFfile_height);
//             for(int i=0;i<fse_count;i++)
//             {
//               entries[i].width =  win_width;
//             }
          //printf("ww=%d ww/4=%d wh=%d\n",win_width, win_width/4, win_height);
          //finish dynamic resize of file list
          refresh_list();
          //list_entries();
          refresh_str_text_dir();
          //refresh_main();
        }
        break;
      case LeaveNotify:
          break;
      case EnterNotify:
          break;
      case ButtonPress:
        if(event.xbutton.button==Button1) 
        {
          // deactivate IFdir by clicking anywhere else
          if(stractive_dir && event.xbutton.window!=IFdir) 
          {
            stractive_dir=0;
            refresh_str_text_dir();
          }
          if(event.xbutton.window == b_ok.w) 
          {
            //abortchoice();
            printf("buttonpress: OK \n");
            button_set_depressed(&b_ok, 1);
            button_toggle(&b_ok);
            //depressed=1;
            //printf("buttonpress_getchoice=%d\n\n",c);
            ///toggle(selected=c);
            //printf("selected=%d\n",c);
          }
          if(event.xbutton.window == b_vol.w) 
          { 
            printf("buttonpress: VOL \n");
            button_set_depressed(&b_vol, 1);
            button_toggle(&b_vol);
          }
          if(event.xbutton.window == b_par.w) 
          { 
            printf("buttonpress: PAR \n");
            button_set_depressed(&b_par, 1);
            button_toggle(&b_par);
          }
          if(event.xbutton.window == b_cancel.w) 
          { 
            printf("buttonpress: CANCEL \n");
            button_set_depressed(&b_cancel, 1);
            button_toggle(&b_cancel);
          }
          
          if(event.xbutton.window==IFdir)
          {
            strbutton_dir(&event.xbutton);
          }
        }
        if(event.xbutton.button==Button4)
        {
          if (!(offset_y > -5))
          {
            offset_y+=5;
            list_entries();
          }
          if (offset_y > -5)
          {
            offset_y = -5;
          }
        }
        if(event.xbutton.button==Button5)
        {
          if (entries_count*16 > list_height)
          {
            offset_y-=5;
            list_entries();
          }
          if (offset_y < -(entries_count*16 - list_height) )
          {
            offset_y = -(entries_count*16 - list_height);
          }
        }

        for(int i=0; i<entries_count;i++)
        {
          if(event.xbutton.window == fse_arr[i].win && event.xbutton.button==Button1)
          {
            printf("selected: path=%s file=%s\n",fse_arr[i].path, fse_arr[i].name);
            if (fse_arr[i].pmA == fse_arr[i].pm1) 
            { 
              fse_arr[i].pmA = fse_arr[i].pm2; 
            }
            else if (fse_arr[i].pmA == fse_arr[i].pm2) 
            { 
              fse_arr[i].pmA = fse_arr[i].pm1; 
            }
            
            XSetWindowBackgroundPixmap(dpy, fse_arr[i].win, fse_arr[i].pmA);
            XClearWindow(dpy,fse_arr[i].win);
            
            if(strcmp(fse_arr[i].type,"directory")==0)
            {
              got_path(fse_arr[i].path);
            }
            else if(strcmp(fse_arr[i].type,"file")==0)
            {
              got_file(fse_arr[i].path,fse_arr[i].name);
            }
          }
        }

        break;
      case ButtonRelease:
        

          
        if(event.xbutton.button==Button1) 
        {
          if(event.xbutton.window == b_ok.w) 
          {
            button_set_depressed(&b_ok, 0);
            button_toggle(&b_ok);
            endchoice(1);
          }
          
          if(event.xbutton.window == b_vol.w) 
          {
            button_set_depressed(&b_vol, 0);
            button_toggle(&b_vol);
            endchoice(2);
          }
          
          if(event.xbutton.window == b_par.w) 
          {
            button_set_depressed(&b_par, 0);
            button_toggle(&b_par);
            endchoice(3);
          }
          if(event.xbutton.window == b_cancel.w) 
          {
            button_set_depressed(&b_cancel, 0);
            button_toggle(&b_cancel);
            endchoice(4);
          }
//           if(depressed) 
//           {
//             endchoice();
//           }
//           else 
//           {
//             abortchoice();
//           }
        }
        break;
      case KeyPress:
        if(stractive_dir) 
        { 
          strkey_dir(&event.xkey);
        }
        if(stractive_file) 
        { 
          strkey_file(&event.xkey); 
        }
        //if(event.xkey.keycode){}
        break;
      case KeyRelease:
        printf("key got released\n");
        break;
      case ClientMessage:
        if (event.xclient.message_type == amiwm_menu &&
          event.xclient.format == 32) 
        {
          unsigned int menunum = event.xclient.data.l[0];
          unsigned menu = menunum & 0x1f;
          unsigned item = (menunum >> 5) & 0x3f;
          unsigned subitem = (menunum >> 11) & 0x1f;
          switch (menu) 
          {
            case 0:
              switch (item) 
              {
                case 0:
                  printf("case 0\n");
                  break;
                case 1:
                  printf("quitting\n");
                  term_dri(&dri, dpy, attr.colormap);
                  exit(0);
                  break;
              }
              break;
            case 1:
              switch (item) 
              {
                case 0:
                  #ifdef AMIGAOS
                  system("RUN <>NIL: requestchoice menutest  ABOUT_STRING  Ok");
                  #else
                  (void)! system("requestchoice >/dev/null menutest ABOUT_STRING Ok &");
                  #endif
                  break;
              }
              break;
          }
        } 
        else if (event.xclient.message_type == wm_protocols &&
          event.xclient.format == 32 &&
          event.xclient.data.l[0] == wm_delete) 
        {
          printf("quitting\n");
          term_dri(&dri, dpy, attr.colormap);
          exit(0);
        }
        break;

    }
  }
//}
}
