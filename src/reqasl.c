#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#include <errno.h>
#ifdef USE_FONTSETS
#include <locale.h>
#include <wchar.h>
#endif

#include "drawinfo.h"
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

#define MAX_CMD_CHARS 256
#define VISIBLE_CMD_CHARS 35

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

static const char ok_txt[]="Ok", vol_txt[]="Volumes", par_txt[]="Parent", cancel_txt[]="Cancel";
static const char drawer_txt[]="Drawer";
static const char file_txt[]="File";

XContext client_context, screen_context, icon_context;

static Window button[8];
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

/**  count max number of FS entries per visited dir */
int fse_count;

/** store screen resolution */
int screen_width=0;
int screen_height=0;

// store browsing progression
char *current_dir="";
char *parent_dir="";

static XIM xim = (XIM) NULL;
static XIC xic = (XIC) NULL;

void refresh_IFborders(Window win);


typedef struct
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
} fs_obj;
fs_obj *entries;  // entries = calloc(fse_count, sizeof(fs_obj));


void clean_reset()
{
    for (int i=0;i<fse_count;i++)
    {
      entries[i].name = None;
      entries[i].path = None;
      entries[i].type = None;
      entries[i].pm1 = None;
      entries[i].pm2 = None;
      entries[i].pmA = None;
      entries[i].win = None;
    }
  XDestroySubwindows(dpy,List); 
  XClearWindow(dpy,List);
  
  fse_count=0;
  free(entries);
}

void read_entries(char *path) {
  // differentiate between files and directories,
  // get max number of instances of wbicon
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(path);
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_DIR || (dp->d_type & DT_REG))
    {
      // exclude common system entries and (semi)hidden names
      if (dp->d_name[0] != '.'){
        fse_count++;
      }
    }
  }
  // get max number of instances of wbicon to allocate
  entries = calloc(fse_count, sizeof(fs_obj));
  closedir(dirp);
  offset_y = 0; // clear scroll offset upon loading a new directory
}

void getlabels(char *path)
{
  // same loop, but for getting filenames this time
  DIR *dirp;
  struct dirent *dp;
  int count=0;
  dirp = opendir(path);
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_DIR)
    {
      if (dp->d_name[0] != '.')
      {
        entries[count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(entries[count].name, dp->d_name);
        int pathsize = strlen(path) + strlen(entries[count].name) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        strcat(tempo,entries[count].name);
        strcat(tempo,"/");
        entries[count].path = tempo;
        //printf("directory, path=%s name=%s\n",entries[count].path,entries[count].name);
        entries[count].type = "directory";
        count++;
      }
    }

    else if (dp->d_type & DT_REG)
    {
      if (dp->d_name[0] != '.')
      {
        //printf ("FILE: %s\n", dp->d_name);
        //wbicon_data(count, dp->d_name, path, "file" );
        entries[count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(entries[count].name, dp->d_name);
        int pathsize = strlen(path) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        entries[count].path = tempo;
        //printf("file, path=%s name=%s\n",entries[count].path,entries[count].name);
        entries[count].type = "file";
        count++;
      }
    }
  }
  closedir(dirp);

  if (entries[0].name == NULL)
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
  if (entries[0].name != NULL)
  {
    char *buf="";
    if(strcmp(entries[0].type, "file")==0)
    {
      current_dir=entries[0].path;
      buf=entries[0].path;  //sample: "/home/klaus/Downlads/icons/"
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

    else if(strcmp(entries[0].type, "directory")==0)
    {
      buf=entries[0].path;
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
}

void list_entries()
{
  //viewmode="list";
  //printf("VIEWMODE now =%s\n",get_viewmode());
  //XClearWindow(dpy,List);
  for (int i=0;i<fse_count;i++)
  {
    entries[i].width  = list_width;
    entries[i].height = win_height-100;
    entries[i].x = 0;
    entries[i].y = 5 + i*16;
    entries[i].y += offset_y;
    XMoveWindow(dpy,entries[i].win,entries[i].x,entries[i].y );
  }
}

void build_entries()
{
  for (int i=0;i<fse_count;i++)
  {

    //int width = XmbTextEscapement(dri.dri_FontSet, entries[i].name, strlen(entries[i].name));
    entries[i].width  = list_width;
    entries[i].height = 15;

    entries[i].win=XCreateSimpleWindow(dpy, List, entries[i].x, entries[i].y, 
                                       entries[i].width, entries[i].height, 0,
                                       dri.dri_Pens[SHADOWPEN],
                                       dri.dri_Pens[BACKGROUNDPEN]);

    entries[i].pm1 = XCreatePixmap(dpy, entries[i].win, entries[i].width, entries[i].height, 24);
    entries[i].pm2 = XCreatePixmap(dpy, entries[i].win, entries[i].width, entries[i].height, 24);

    XSetForeground(dpy, gc, 0xaaaaaa);
    XFillRectangle(dpy, entries[i].pm1, gc, 0, 0, entries[i].width, entries[i].height);

    if (strcmp(entries[i].type,"directory")==0)
    {
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
      XFillRectangle(dpy, entries[i].pm1, gc, 0, 0, entries[i].width, entries[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, entries[i].pm1, gc, win_width-75, 12, "Drawer", strlen("Drawer"));
      XDrawImageString(dpy, entries[i].pm1, gc, 5, 12, entries[i].name, strlen(entries[i].name));
      XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);

      XSetBackground(dpy, gc, dri.dri_Pens[FILLPEN]);
      XFillRectangle(dpy, entries[i].pm2, gc, 0, 0, entries[i].width, entries[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, entries[i].pm2, gc, win_width-75, 12, "Drawer", strlen("Drawer"));
      XDrawImageString(dpy, entries[i].pm2, gc, 5, 12, entries[i].name, strlen(entries[i].name));
    }
    else if (strcmp(entries[i].type,"file")==0)
    {
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
      XFillRectangle(dpy, entries[i].pm1, gc, 0, 0, entries[i].width, entries[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, entries[i].pm1, gc, win_width-65, 12, "---", strlen("---"));
      XDrawImageString(dpy, entries[i].pm1, gc, 5, 12, entries[i].name, strlen(entries[i].name));
      XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);

      XSetBackground(dpy, gc, dri.dri_Pens[FILLPEN]);
      XFillRectangle(dpy, entries[i].pm2, gc, 0, 0, entries[i].width, entries[i].height);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, entries[i].pm2, gc, win_width-65, 12, "---", strlen("---"));
      XDrawImageString(dpy, entries[i].pm2, gc, 5, 12, entries[i].name, strlen(entries[i].name));
    }
    XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    entries[i].pmA = entries[i].pm1; // set active pixmap
    XSetWindowBackgroundPixmap(dpy, entries[i].win, entries[i].pmA);

    XSelectInput(dpy, entries[i].win, ExposureMask|CWOverrideRedirect|KeyPressMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|ShiftMask);
  }
  XMapSubwindows(dpy,List);
}

/** get button number/id */
int getchoice(Window w)
{
  int i;
  int totalbuttons = (int)(sizeof button / sizeof button[0]);
  for(i=1; i<totalbuttons; i++)
  {
    if(button[i]==w)
    {
      printf("button id=%d\n",i);
      return i;
    }
  }
  return 0;
}

/** refresh buttons in window */
void refresh_button(Window w, const char *txt, int idx)
{
  //printf("refresh_button \n");
  int h=fh+2*BUT_VSPACE;
  int l=strlen(txt);
  int tw=XmbTextEscapement(dri.dri_FontSet, txt, l);
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  XDrawString(dpy, w, gc, (butw-tw)>>1, dri.dri_Ascent+BUT_VSPACE, txt, l);
  XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)? SHADOWPEN:SHINEPEN]);
  XDrawLine(dpy, w, gc, 0, 0, butw-2, 0);
  XDrawLine(dpy, w, gc, 0, 0, 0, h-2);
  XSetForeground(dpy, gc, dri.dri_Pens[(selected==idx && depressed)? SHINEPEN:SHADOWPEN]);
  XDrawLine(dpy, w, gc, 1, h-1, butw-1, h-1);
  XDrawLine(dpy, w, gc, butw-1, 1, butw-1, h-1);
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XDrawPoint(dpy, w, gc, butw-1, 0);
  XDrawPoint(dpy, w, gc, 0, h-1);
  //printf("refresh_idx=%d\n",idx);
}

/** refresh window background */
void refresh_main(void)
{
  int w;
  w=XmbTextEscapement(dri.dri_FontSet, drawer_txt, strlen(drawer_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 10, win_height-60, drawer_txt, strlen(drawer_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, win_height-36, file_txt, strlen(file_txt));

}


/** refresh text string in input field (typing) */
extern size_t mbrlen(); // define proto mbrlen() to silence ccompile warning
void refresh_str_text_dir()
{
  //printf("refreshing ifdir string: current_path=%s\n", current_dir);
  
  //dir_path=current_dir;
  int string_length=XmbTextEscapement(dri.dri_FontSet, current_dir, strlen(current_dir)); 
  
  //clean
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, IFdir, gc, 0, 3,IFdir_width, fh);
  
  //write    
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(string_length>IFdir_width)
  {
    int offset_x = string_length - IFdir_width;
    XmbDrawImageString(dpy, IFdir, dri.dri_FontSet,
                      gc, -(offset_x +8 ), 3+dri.dri_Ascent,
                       current_dir, strlen(current_dir));
  } 
  else 
  {
    XmbDrawImageString(dpy, IFdir, dri.dri_FontSet,
                        gc, 6, 3+dri.dri_Ascent,
                        current_dir, strlen(current_dir)); 
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

void refresh_str_text_file()
{
  
  refresh_IFborders(IFfile);
}

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

/** work with text string entered */
void strkey_dir(XKeyEvent *e)
{
  void endchoice(void);
  Status stat;
  KeySym ks;
  
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

/** work with text string entered */
void strkey_file(XKeyEvent *e)
{

}

/** click on input field (stractive) */
void strbutton_dir(XButtonEvent *e)
{
  stractive_dir=1;

  int w, l=1;
  //stractive=1;
  cur_pos_dir=left_pos_dir;
  cur_x=6;
  while(cur_x<e->x && cur_pos_dir<buf_len_dir) {

    l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
    if(l<=0)
      break;
    w=XmbTextEscapement(dri.dri_FontSet, dir_path+cur_pos_dir, l);

    if(cur_x+w>e->x)
      break;
    cur_x+=w;
    cur_pos_dir+=l;
  }
  //printf("");
  refresh_str_text_dir();
}

/** click on input field (stractive) */
void strbutton_file(XButtonEvent *e)
{
}

void toggle(int c)
{
  XSetWindowBackground(dpy, button[c],
                       dri.dri_Pens[(depressed&&c==selected)?
                       FILLPEN:BACKGROUNDPEN]);
  XClearWindow(dpy, button[c]);
  refresh_button(button[c], buttxt[c], c);
  //printf("toggle = %d\n\n",c);
}

void toggle_choice(int c)
{
  XSetWindowBackground(dpy, button[c],
                       dri.dri_Pens[(depressed&&c==selected)?
                       FILLPEN:BACKGROUNDPEN]);
  XClearWindow(dpy, button[c]);
  refresh_button(button[c], buttxt[c], c);
}

void abortchoice()
{
  if(depressed) {
    depressed=0;
    toggle(selected);
  }
  selected=0;
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
  read_entries(path);
  getlabels(path);
  build_entries();
  list_entries();
  current_dir=path;
  refresh_str_text_dir();
  refresh_str_text_file();
  }
  else {printf("input path do not exists\n\n");}
}

void endchoice()
{
  int c=selected;
  abortchoice();

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

void force_first_refresh()
{
  refresh_str_text_dir();
//   XEvent event;
//   XNextEvent(dpy,&event);
}


int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  // clickable buttons
  Window b_ok, b_vol, b_par, b_cancel;
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

  // width and height of input field borders
  strgadw=win_width-70;
  strgadh=(fh=dri.dri_Ascent+dri.dri_Descent)+6;

  butw=XmbTextEscapement(dri.dri_FontSet, ok_txt, strlen(ok_txt))+2*BUT_HSPACE;
  w2=XmbTextEscapement(dri.dri_FontSet, cancel_txt, strlen(cancel_txt))+2*BUT_HSPACE;
  if(w2>butw)
    butw=w2;

  mainw=2*(BUT_SIDE+butw)+BUT_SIDE;
  w2=XmbTextEscapement(dri.dri_FontSet, drawer_txt, strlen(drawer_txt))+2*TEXT_SIDE;
  if(w2>mainw)
    mainw=w2;

  w2=strgadw+XmbTextEscapement(dri.dri_FontSet, file_txt, strlen(file_txt))+
  2*TEXT_SIDE+2*BUT_SIDE+butw;
  if(w2>mainw)
    mainw=w2;

  mainh=3*fh+TOP_SPACE+BOT_SPACE+2*INT_SPACE+2*BUT_VSPACE;

  int s_middle_x = (screen_width/2) - win_width/2;
  int s_middle_y = (screen_height/2) - win_height;

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

  b_ok=XCreateSimpleWindow(dpy, mainwin,BUT_SIDE,
                           mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                           butw, fh+2*BUT_VSPACE, 0,
                           dri.dri_Pens[SHADOWPEN],
                           dri.dri_Pens[BACKGROUNDPEN]);

  b_vol=XCreateSimpleWindow(dpy, mainwin, BUT_SIDE,
                            mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                            butw, fh+2*BUT_VSPACE, 0,
                            dri.dri_Pens[SHADOWPEN],
                            dri.dri_Pens[BACKGROUNDPEN]);

  b_par=XCreateSimpleWindow(dpy, mainwin, BUT_SIDE,
                            mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                            butw, fh+2*BUT_VSPACE, 0,
                            dri.dri_Pens[SHADOWPEN],
                            dri.dri_Pens[BACKGROUNDPEN]);

  b_cancel=XCreateSimpleWindow(dpy, mainwin, BUT_SIDE,
                               mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                               butw, fh+2*BUT_VSPACE, 0,
                               dri.dri_Pens[SHADOWPEN],
                               dri.dri_Pens[BACKGROUNDPEN]);
  button[0]=None;
  button[1]=b_ok;
  button[2]=b_vol;
  button[3]=b_par;
  button[4]=b_cancel;

  //totalbuttons = sizeof button / sizeof button[0];
  //printf("totalbuttonlength=%d\n",(int)(sizeof button / sizeof button[0]));
  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, List, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, IFdir, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, IFfile, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, b_ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_vol, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_par, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_cancel, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);



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

  input=IFdir;

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
  got_path(homedir);
  got_path(homedir);
//   read_entries(homedir);
//   getlabels(homedir);
//   build_entries();
//   list_entries(); 
//   refresh_str_text_dir();
//   refresh_str_text_file();
  force_first_refresh();
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);

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
          if(!event.xexpose.count) 
          {
            if(event.xexpose.window == List) 
            {
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
            if(event.xexpose.window == b_ok) 
            {
              refresh_button(b_ok, ok_txt, 1);
            }
            if(event.xexpose.window == b_vol) 
            {
              refresh_button(b_vol, vol_txt, 2);
            }
            if(event.xexpose.window == b_par) 
            {
              refresh_button(b_par, par_txt, 3);
            }
            if(event.xexpose.window == b_cancel) 
            {
              refresh_button(b_cancel, cancel_txt, 4);
            }
            if(event.xexpose.window == mainwin) 
            {
              refresh_main();
            }
          }
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
            XMoveWindow(dpy, b_ok, button_spread(0,4)+5, event.xconfigure.height - 25);       // 10
            XMoveWindow(dpy, b_vol, button_spread(1,4)+5, event.xconfigure.height - 25);      // 70
            XMoveWindow(dpy, b_par, button_spread(2,4)+5, event.xconfigure.height - 25);     // 130
            XMoveWindow(dpy, b_cancel,button_spread(3,4)+5 , event.xconfigure.height - 25); // 190
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
            if((c=getchoice(event.xbutton.window))) 
            {
              abortchoice();
              depressed=1;
              printf("buttonpress_getchoice=%d\n\n",c);
              toggle(selected=c);
              //printf("selected=%d\n",c);
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
            if (fse_count*16 > list_height)
            {
              offset_y-=5;
              list_entries();
            }
            if (offset_y < -(fse_count*16 - list_height) )
            {
              offset_y = -(fse_count*16 - list_height);
            }
          }

          for(int i=0; i<fse_count;i++)
          {
            if(event.xbutton.window == entries[i].win && event.xbutton.button==Button1)
            {
              printf("selected: path=%s file=%s\n",entries[i].path, entries[i].name);
              if (entries[i].pmA == entries[i].pm1) 
              { 
                entries[i].pmA = entries[i].pm2; 
              }
              else if (entries[i].pmA == entries[i].pm2) 
              { 
                entries[i].pmA = entries[i].pm1; 
              }
              
              XSetWindowBackgroundPixmap(dpy, entries[i].win, entries[i].pmA);
              XClearWindow(dpy,entries[i].win);
              
              if(strcmp(entries[i].type,"directory")==0)
              {
                got_path(entries[i].path);
              }
              else if(strcmp(entries[i].type,"file")==0)
              {
                got_file(entries[i].path,entries[i].name);
              }
            }
          }

          break;
        case ButtonRelease:
          if(event.xbutton.button==Button1 && selected) 
          {
            if(depressed) 
            {
              endchoice();
            }
            else 
            {
              abortchoice();
            }
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
      }
    }
  //}
}
