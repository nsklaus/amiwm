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
char dir_path[MAX_CMD_CHARS+1];
char file_path[MAX_CMD_CHARS+1];
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

Window root, mainwin, IFdir, IFfile, List, input;
int win_x=20, win_y=20, win_width=262, win_height=250;
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
  XClearWindow(dpy,mainwin);
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

void list_entries()
{
  //viewmode="list";
  //printf("VIEWMODE now =%s\n",get_viewmode());
  XClearWindow(dpy,List);
  for (int i=0;i<fse_count;i++)
  {
    entries[i].width  = win_width-30;
    entries[i].height = win_height-100;
    entries[i].x = 10;
    entries[i].y = 15 + i*16;
    entries[i].y += offset_y;
    XMoveWindow(dpy,entries[i].win,entries[i].x,entries[i].y );
  }
}

void build_entries()
{
  for (int i=0;i<fse_count;i++)
  {

    int width = XmbTextEscapement(dri.dri_FontSet, entries[i].name, strlen(entries[i].name));
    entries[i].width  = win_width-40;
    entries[i].height = 15;

    entries[i].win=XCreateSimpleWindow(dpy, List, entries[i].x, entries[i].y, entries[i].width, entries[i].height, 0,
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
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len_dir>left_pos_dir) {
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
  if(stractive_dir) {
    if(cur_pos_dir<buf_len_dir) {
      XSetBackground(dpy, gc, ~0);

      l=mbrlen(dir_path+cur_pos_dir, buf_len_dir-cur_pos_dir, NULL);
      XmbDrawImageString(dpy, input, dri.dri_FontSet,
                         gc, cur_x, 3+dri.dri_Ascent,
                         dir_path+cur_pos_dir, l);
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    }
    else {
      XSetForeground(dpy, gc, ~0);
      XFillRectangle(dpy, input, gc, cur_x, 3,
                     XExtentsOfFontSet(dri.dri_FontSet)->
                     max_logical_extent.width, fh);
    }
  }
}

void refresh_str_text_file()
{
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len_file>left_pos_file) {
    int w, c;
    for(l=0; l<buf_len_file-left_pos_file; ) {
      c=mbrlen(file_path+left_pos_file+l, buf_len_file-left_pos_file-l, NULL);
      w=6+XmbTextEscapement(dri.dri_FontSet, file_path+left_pos_file, l+c);
      if(w>strgadw-6)
        break;
      mx=w;
      l+=c;
    }

    XmbDrawImageString(dpy, input, dri.dri_FontSet,
                       gc, 6, 3+dri.dri_Ascent,
                       file_path+left_pos_file, l);
  }
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, input, gc, mx, 3, strgadw-mx-6, fh);
  if(stractive_file) {
    if(cur_pos_file<buf_len_file) {
      XSetBackground(dpy, gc, ~0);

      l=mbrlen(file_path+cur_pos_file, buf_len_file-cur_pos_file, NULL);
      XmbDrawImageString(dpy, input, dri.dri_FontSet,
                         gc, cur_x, 3+dri.dri_Ascent,
                         file_path+cur_pos_file, l);
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    }
    else {
      XSetForeground(dpy, gc, ~0);
      XFillRectangle(dpy, input, gc, cur_x, 3,
                     XExtentsOfFontSet(dri.dri_FontSet)->
                     max_logical_extent.width, fh);
    }
  }
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
void refresh_str_dir()
{

  refresh_str_text_dir();
  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, IFdir, gc, 0, strgadh-1, 0, 0);
  XDrawLine(dpy, IFdir, gc, 0, 0, strgadw-2, 0);
  XDrawLine(dpy, IFdir, gc, 3, strgadh-2, strgadw-4, strgadh-2);
  XDrawLine(dpy, IFdir, gc, strgadw-4, strgadh-2, strgadw-4, 2);
  XDrawLine(dpy, IFdir, gc, 1, 1, 1, strgadh-2);
  XDrawLine(dpy, IFdir, gc, strgadw-3, 1, strgadw-3, strgadh-2);
  XSetForeground(dpy, gc, dri.dri_Pens[SHADOWPEN]);
  XDrawLine(dpy, IFdir, gc, 1, strgadh-1, strgadw-1, strgadh-1);
  XDrawLine(dpy, IFdir, gc, strgadw-1, strgadh-1, strgadw-1, 0);
  XDrawLine(dpy, IFdir, gc, 3, strgadh-3, 3, 1);
  XDrawLine(dpy, IFdir, gc, 3, 1, strgadw-4, 1);
  XDrawLine(dpy, IFdir, gc, 2, 1, 2, strgadh-2);
  XDrawLine(dpy, IFdir, gc, strgadw-2, 1, strgadw-2, strgadh-2);
}

/** refresh drawing of text field borders */
void refresh_str_file()
{

  refresh_str_text_file();

  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, IFfile, gc, 0, strgadh-1, 0, 0);
  XDrawLine(dpy, IFfile, gc, 0, 0, strgadw-2, 0);
  XDrawLine(dpy, IFfile, gc, 3, strgadh-2, strgadw-4, strgadh-2);
  XDrawLine(dpy, IFfile, gc, strgadw-4, strgadh-2, strgadw-4, 2);
  XDrawLine(dpy, IFfile, gc, 1, 1, 1, strgadh-2);
  XDrawLine(dpy, IFfile, gc, strgadw-3, 1, strgadw-3, strgadh-2);
  XSetForeground(dpy, gc, dri.dri_Pens[SHADOWPEN]);
  XDrawLine(dpy, IFfile, gc, 1, strgadh-1, strgadw-1, strgadh-1);
  XDrawLine(dpy, IFfile, gc, strgadw-1, strgadh-1, strgadw-1, 0);
  XDrawLine(dpy, IFfile, gc, 3, strgadh-3, 3, 1);
  XDrawLine(dpy, IFfile, gc, 3, 1, strgadw-4, 1);
  XDrawLine(dpy, IFfile, gc, 2, 1, 2, strgadh-2);
  XDrawLine(dpy, IFfile, gc, strgadw-2, 1, strgadw-2, strgadh-2);
}
/** work with text string entered */
void strkey_dir(XKeyEvent *e)
{
  void endchoice(void);
  Status stat;
  KeySym ks;

  //char buf[256];

  char buf_dir[256];

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
}

/** work with text string entered */
void strkey_file(XKeyEvent *e)
{
  void endchoice(void);
  Status stat;
  KeySym ks;

  char buf_file[256];

  int x, i, n;
  n=XmbLookupString(xic, e, buf_file, sizeof(buf_file), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)

    switch(ks) {
      case XK_Return:
      case XK_Linefeed:
        selected=1;
        endchoice();
        break;
      case XK_Left:
        if(cur_pos_file) {
          int p=cur_pos_file;
          while(p>0) {
            --p;
            if(((int)mbrlen(file_path+p, cur_pos_file-p, NULL))>0) {
              cur_pos_file=p;
              break;
            }
          }
        }
        break;
      case XK_Right:
        if(cur_pos_file<buf_len_file) {
          int l=mbrlen(file_path+cur_pos_file, buf_len_file-cur_pos_file, NULL);
          if(l>0)
            cur_pos_file+=l;
        }
        break;
      case XK_Begin:
        cur_pos_file=0;
        break;
      case XK_End:
        cur_pos_file=buf_len_file;
        break;
      case XK_Delete:
        if(cur_pos_file<buf_len_file) {
          int l=1;
          l=mbrlen(file_path+cur_pos_file, buf_len_file-cur_pos_file, NULL);
          if(l<=0)
            break;
          buf_len_file-=l;
          for(x=cur_pos_file; x<buf_len_file; x++){
            file_path[x]=file_path[x+l];
          }
          file_path[x] = 0;
        }
        else {
          XBell(dpy, 100);
        }
        break;
      case XK_BackSpace:
        if(cur_pos_file>0) {
          int l=1;
          int p=cur_pos_file;
          while(p>0) {
            --p;
            if(((int)mbrlen(file_path+p, cur_pos_file-p, NULL))>0) {
              l=cur_pos_file-p;
              break;
            }
          }
          buf_len_file-=l;
          for(x=(cur_pos_file-=l); x<buf_len_file; x++) {
            file_path[x]=file_path[x+l];
          }
          file_path[x] = 0;
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
    for(i=0; i<n && buf_len_file<MAX_CMD_CHARS; i++) {
      for(x=buf_len_file; x>cur_pos_file; --x)
        file_path[x]=file_path[x-1];
      file_path[cur_pos_file++]=buf_file[i];
      buf_len_file++;
    }
    if(i<n)
      XBell(dpy, 100);
  }
  if(cur_pos_file<left_pos_file)
    left_pos_file=cur_pos_file;
  cur_x=6;

  if(cur_pos_file>left_pos_file)
    cur_x+=XmbTextEscapement(dri.dri_FontSet, file_path+left_pos_file, cur_pos_file-left_pos_file);
  if(cur_pos_file<buf_len_file) {
    int l=mbrlen(file_path+cur_pos_file, buf_len_file-cur_pos_file, NULL);
    x=XmbTextEscapement(dri.dri_FontSet, file_path+cur_pos_file, l);
  } else
    x=XExtentsOfFontSet(dri.dri_FontSet)->max_logical_extent.width;

  if((x+=cur_x-(strgadw-6))>0) {
    cur_x-=x;
    while(x>0) {
      int l=mbrlen(file_path+left_pos_file, buf_len_file-left_pos_file, NULL);
      x-=XmbTextEscapement(dri.dri_FontSet, file_path+left_pos_file, l);
      left_pos_file += l;
    }
    cur_x+=x;
  }
  refresh_str_text_file();
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
  refresh_str_dir();
}

/** click on input field (stractive) */
void strbutton_file(XButtonEvent *e)
{
  stractive_file=1;

  int w, l=1;
  //stractive=1;
  cur_pos_file=left_pos_file;
  cur_x=6;
  while(cur_x<e->x && cur_pos_file<buf_len_file) {

    l=mbrlen(file_path+cur_pos_file, buf_len_file-cur_pos_file, NULL);
    if(l<=0)
      break;
    w=XmbTextEscapement(dri.dri_FontSet, file_path+cur_pos_file, l);

    if(cur_x+w>e->x)
      break;
    cur_x+=w;
    cur_pos_file+=l;
  }
  refresh_str_file();
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

void got_path(char *path)
{
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
    else if(input == IFfile)
      got_path(file_path);
  }
  if(c==2){
    printf("volumes\n");
    current_dir="/";
    parent_dir="/";
    printf("(v) current path=/\n");
    printf("(v) parent path=/\n\n");
    clean_reset();
    read_entries("/");
    getlabels("/");
    build_entries();
    list_entries();
  }
  if(c==3){
    printf("parent\n");

    /*
    char *buf;
    if(strcmp(entries[0].type, "file")==0){
      buf=entries[0].path;  //sample: "/home/klaus/Downlads/icons/"
      printf("FILE_ORIG: buf_len=%lu buf=%s\n",strlen(buf),buf);
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
        printf("FILE_FINAL: len_of_newbuff=%lu newbuff=%s \n",strlen(newbuff),newbuff);
        buf=newbuff;
      }
    }
    else if(strcmp(entries[0].type, "directory")==0)
    {
      buf=entries[0].path;
      printf("DIR_ORIG: buf_len=%lu buf=%s\n",strlen(buf),buf);
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
        printf("DIR_FINAL: len_of_newbuff=%lu newbuff=%s \n",strlen(newbuff),newbuff);
        buf=newbuff;
      }
    }

    if(strchr(buf, '/') != NULL) { }
    */

    //XClearWindow(dpy,List);
    if(parent_dir != NULL)
    {
      if (strcmp(parent_dir,"/")==0) {
        printf("(p) current dir=/\n");
        printf("(p) parent dir=/\n\n");
      }
      //printf("(p) --- parent dir=%s\n\n",parent_dir);
      clean_reset();
      read_entries(parent_dir);
      getlabels(parent_dir);
      build_entries();
      list_entries();
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
  List=XCreateSimpleWindow(dpy, mainwin, 10, 10, win_width-20, win_height-95, 0,
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
  read_entries(homedir);
  getlabels(homedir);
  build_entries();
  list_entries();
  XMapSubwindows(dpy,List);
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);

  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    //#ifdef USE_FONTSETS
    if(!XFilterEvent(&event, mainwin)) {
      //#endif
      switch(event.type) {
        case Expose:
          if(!event.xexpose.count) {

            if(event.xexpose.window == IFdir) {
              input=IFdir;
              refresh_str_dir();
            }
            if(event.xexpose.window == IFfile) {
              input=IFfile;
              refresh_str_file();
            }
            if(event.xexpose.window == List) {
              refresh_list();
            }
            if(event.xexpose.window == b_ok) {
              refresh_button(b_ok, ok_txt, 1);
            }
            if(event.xexpose.window == b_vol) {
              refresh_button(b_vol, vol_txt, 2);
            }
            if(event.xexpose.window == b_par) {
              refresh_button(b_par, par_txt, 3);
            }
            if(event.xexpose.window == b_cancel) {
              refresh_button(b_cancel, cancel_txt, 4);
            }
            if(event.xexpose.window == mainwin) {
              refresh_main();
            }
          }
        case ConfigureNotify:
          if(event.xconfigure.window == mainwin) {
            // make save and cancel button to stay at bottom of window when it gets resized
            win_x=event.xconfigure.x;
            win_y=event.xconfigure.y;
            if (event.xconfigure.width < 255) {
              win_width=255;
            }
            else {
              win_width=event.xconfigure.width;
            }
            win_height=event.xconfigure.height;
            strgadw=win_width-70;
            XMoveWindow(dpy, IFdir, 60, win_height-73);
            XMoveWindow(dpy, IFfile, 60, win_height-50);
            XMoveWindow(dpy, b_ok, button_spread(0,4)+5, event.xconfigure.height - 25);       // 10
            XMoveWindow(dpy, b_vol, button_spread(1,4)+5, event.xconfigure.height - 25);      // 70
            XMoveWindow(dpy, b_par, button_spread(2,4)+5, event.xconfigure.height - 25);     // 130
            XMoveWindow(dpy, b_cancel,button_spread(3,4)+5 , event.xconfigure.height - 25); // 190
            XResizeWindow(dpy,List,win_width-20, win_height-95);
            XResizeWindow(dpy,IFdir,win_width-70, 20);
            XResizeWindow(dpy,IFfile,win_width-70, 20);
            for(int i=0;i<fse_count;i++)
            {
              entries[i].width =  win_width;
            }
            //printf("ww=%d ww/4=%d wh=%d\n",win_width, win_width/4, win_height);
            //finish dynamic resize of file list
            refresh_list();
            list_entries();
            refresh_str_dir();
            refresh_str_file();
            refresh_main();
          }
          break;
        case LeaveNotify:
          //printf("leavenotify=%d\n\n",c);
          if(depressed &&
            event.xcrossing.window==button[c]) {
            depressed=0;
          printf("leavenotify=%d  selected=%d\n\n",c, selected);
          toggle(selected);
            }
            break;
        case EnterNotify:
          //printf("enternotify=%d\n\n",c);
          if((!depressed) && selected &&
            event.xcrossing.window==button[selected]) {
            depressed=1;
          printf("enternotify=%d\n\n",selected);
          toggle(selected);
            }
            break;
        case ButtonPress:
          if(event.xbutton.button==Button1) {
            if(stractive_dir && event.xbutton.window!=IFdir) {
              stractive_dir=0;
              refresh_str_dir();
            }
            if(stractive_file && event.xbutton.window!=IFfile) {
              stractive_file=0;
              refresh_str_file();
            }
            if((c=getchoice(event.xbutton.window))) {
              abortchoice();
              depressed=1;
              printf("buttonpress_getchoice=%d\n\n",c);
              toggle(selected=c);
              //printf("selected=%d\n",c);
            }
            if(event.xbutton.window==IFdir) {
              input=IFdir;
              strbutton_dir(&event.xbutton);
            }
            if(event.xbutton.window==IFfile) {
              input=IFfile;
              strbutton_file(&event.xbutton);
            }
          }
          if(event.xbutton.button==Button4)
          {
            //printf("going up y=%d\n",event.xconfigure.height);
            offset_y+=5;
            list_entries();

          }
          if(event.xbutton.button==Button5)
          {
            //printf("going down y=%d\n",event.xconfigure.height);
            offset_y-=5;
            list_entries();
          }

          for(int i=0; i<fse_count;i++)
          {
            if(event.xbutton.window == entries[i].win && event.xbutton.button==Button1)
            {
              printf("selected: %s\n",entries[i].name);
              if (entries[i].pmA == entries[i].pm1) { entries[i].pmA = entries[i].pm2; }
              else if (entries[i].pmA == entries[i].pm2) { entries[i].pmA = entries[i].pm1; }

              XSetWindowBackgroundPixmap(dpy, entries[i].win, entries[i].pmA);
              XClearWindow(dpy,entries[i].win);
              XFlush(dpy);
            }
          }

          // if(event.xbutton.window==List) {}

          break;
        case ButtonRelease:
          if(event.xbutton.button==Button1 && selected) {
            if(depressed) {
              endchoice();
            }
            else {
              abortchoice();
            }
          }
          break;
        case KeyPress:
          if(stractive_dir) { strkey_dir(&event.xkey); }
          if(stractive_file) { strkey_file(&event.xkey); }
          //if(event.xkey.keycode){}
          break;
        case KeyRelease:
          printf("key got released\n");
          break;
      }
    }
  }
}
