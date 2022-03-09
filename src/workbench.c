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
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#include <errno.h>
#ifdef USE_FONTSETS
#include <locale.h>
#include <wchar.h>
#endif

#include "drawinfo.h"
#include <dirent.h>
#include "libami.h"
#include <sys/stat.h>
#include <math.h>
#include "screen.h"

int dblClickTime=400;
Time last_icon_click=0, last_double=0;

static int selected=0, depressed=0, stractive=1;
struct ColorStore colorstore1, colorstore2;

char *progname;
Display *dpy;

struct DrawInfo dri;

typedef struct
{
  char *name;
  char *path;
  char *type; // file or dir
  Window iconwin;
  Pixmap pm1;
  Pixmap pm2;
  Pixmap pmA; // active pixmap
  int x;
  int y;
  int width;
  int height;
  Bool selected;
} wbicon;

wbicon *icons;

int dircount;
Window root, mainwin;//, myicon;
int win_x=20, win_y=20, win_width=300, win_height=150;
GC gc;

void wbicon_data(int count, char *name, char *path, char *type)
{
  // need to be redone
  icons[count].name =  malloc(strlen(name)+1);
  strcpy(icons[count].name, name);

  int pathsize = strlen(path) + strlen(icons[count].name) +1;
  char *tempo = malloc(pathsize);

  strcpy(tempo,path);
  strcat(tempo,icons[count].name);
  strcat(tempo,"/");
  icons[count].path = malloc(pathsize);
  icons[count].path = tempo;
  icons[count].type = type;
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
        dircount++;
      }
    }
  }
  // get max number of instances of wbicon to allocate
  icons = calloc(dircount, sizeof(wbicon));
  closedir(dirp);
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
        icons[count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(icons[count].name, dp->d_name);
        int pathsize = strlen(path) + strlen(icons[count].name) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        strcat(tempo,icons[count].name);
        strcat(tempo,"/");
        icons[count].path = tempo;
        icons[count].type = "directory";
        count++;
      }
    }

    else if (dp->d_type & DT_REG)
    {
      if (dp->d_name[0] != '.')
      {
        //printf ("FILE: %s\n", dp->d_name);
        //wbicon_data(count, dp->d_name, path, "file" );
        icons[count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(icons[count].name, dp->d_name);
        int pathsize = strlen(path) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        icons[count].path = tempo;
        icons[count].type = "file";
        count++;
      }
    }
  }
  closedir(dirp);
}

void list_entries()
{
  // icon palette
  unsigned long iconcolor[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };

  int newline_x = 0;
  int newline_y = 0;
  struct DiskObject *icon_do = NULL;
  char *icondir="/usr/local/lib/amiwm/icons";
  char *icon = "def_drawer.info";//="def_tool.info";
  for (int i=0;i<dircount;i++)
  {
    if(strcmp(icons[i].type, "directory") == 0) { icon="def_drawer.info"; }
    else if(strcmp(icons[i].type, "file") == 0) { icon="def_tool.info"; }

    if (icon != NULL && *icon != 0)
    {
      int rl=strlen(icon)+strlen(icondir)+2;
      char *fn=alloca(rl);
      sprintf(fn, "%s/%s", icondir, icon);
      fn[strlen(fn)-5]=0;
      //printf("fn=%s\n",fn);
      icon_do = GetDiskObject(fn);
    }

    struct Image *im1 = icon_do->do_Gadget.GadgetRender;
    struct Image *im2 = icon_do->do_Gadget.SelectRender;

    Pixmap pm1, pm2;

      icons[i].width=icon_do->do_Gadget.Width;
      icons[i].height=icon_do->do_Gadget.Height;

      if(newline_x*80 < win_width)
      {
        icons[i].x=10 + (newline_x*80);
        icons[i].y=10 + (newline_y*50);
        newline_x++;
      }
      else if(newline_x*80 > win_width)
      {
        newline_x = 0;
        newline_y++;
        icons[i].x=10 + (newline_x*80);
        icons[i].y=10 + (newline_y*50);
        newline_x++;
      }
      icons[i].iconwin=XCreateSimpleWindow(dpy, mainwin, icons[i].x, icons[i].y,
                                          icons[i].width+18, icons[i].height+15, 1,
                                           dri.dri_Pens[BACKGROUNDPEN],//TEXTPEN], //
                                          dri.dri_Pens[BACKGROUNDPEN]);

      pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                            im1, icons[i].width, icons[i].height, &colorstore1);
      pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                            im2, icons[i].width, icons[i].height, &colorstore2);

      icons[i].pm1 = XCreatePixmap(dpy, pm1, icons[i].width+18, icons[i].height+15, 24);
      XFillRectangle(dpy,icons[i].pm1, gc, 0,0,icons[i].width+18, icons[i].height+15);
      XCopyArea(dpy, pm1, icons[i].pm1, gc, -9, 0, icons[i].width+18, icons[i].height, 0, 0);

      icons[i].pm2 = XCreatePixmap(dpy, pm2, icons[i].width+18, icons[i].height+15, 24);
      XFillRectangle(dpy,icons[i].pm2, gc, 0,0,icons[i].width+18, icons[i].height+15);
      XCopyArea(dpy, pm2, icons[i].pm2, gc, -9, 0, icons[i].width+18, icons[i].height, 0, 0);


      icons[i].pmA = icons[i].pm1; /* set active pixmap */
      XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);

      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);

      //shorten long labels
      if (strlen(icons[i].name) > 10 )
      {
        char *str1=icons[i].name;
        char str2[i][12];
        strncpy (str2[i],str1,9);
        str2[i][9]='.';
        str2[i][10]='.';
        str2[i][11]='\0';
        XmbDrawString(dpy, icons[i].pm1, dri.dri_FontSet, gc, 0, icons[i].height+10, str2[i], strlen(str2[i]));
        XmbDrawString(dpy, icons[i].pm2, dri.dri_FontSet, gc, 0, icons[i].height+10, str2[i], strlen(str2[i]));
      }
      else
      {
        // get string length in pixels and calc offset
        int my_offset = XmbTextEscapement(dri.dri_FontSet, icons[i].name, strlen(icons[i].name));
        int new_offset = (icons[i].width+18 - my_offset)/2;
        XmbDrawString(dpy, icons[i].pm1, dri.dri_FontSet, gc, new_offset, icons[i].height+10, icons[i].name, strlen(icons[i].name));
        XmbDrawString(dpy, icons[i].pm2, dri.dri_FontSet, gc, new_offset, icons[i].height+10, icons[i].name, strlen(icons[i].name));
      }
      XSelectInput(dpy, icons[i].iconwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  }
  // XFreePixmap(dpy, pm1);
  // XFreePixmap(dpy, pm2);
  if(icon_do != NULL){ FreeDiskObject(icon_do); }
}

void spawn_new_wb(const char *cmd, char *title)
{
  const char *exec = "/usr/local/lib/amiwm/workbench";
  char *line=alloca(strlen(exec) + strlen(cmd) + strlen(title) +4);
  sprintf(line, "%s %s %s &", exec, cmd, title);
  //printf("spawn_new_wb: my exec line=%s\n",line);
  system(line);
}

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }

  // set default window title
  if(argc<2) {  argv[2]= "home"; }

  // open default directory
  if(argv[1]==NULL) { argv[1]= "/home/klaus/"; }

  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  mainwin=XCreateSimpleWindow(dpy, root, win_x, win_y, win_width, win_height, 1,
                              dri.dri_Pens[BACKGROUNDPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);

  size_hints.flags = USSize;
  txtprop1.value=(unsigned char *)argv[2];
  txtprop2.value=(unsigned char *)argv[2];
  txtprop2.encoding=txtprop1.encoding=XA_STRING;
  txtprop2.format=txtprop1.format=8;
  txtprop1.nitems=strlen((char *)txtprop1.value);
  txtprop2.nitems=strlen((char *)txtprop2.value);
  XSetWMProperties(dpy, mainwin,
                  &txtprop1,
                  &txtprop2, argv, argc,
                  &size_hints, NULL, NULL);

  read_entries(argv[1]);
  getlabels(argv[1]);  /* todo: try to remove one of the two loop  */
  list_entries();

  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  XSync(dpy, False);

  for(;;)
  {
    XEvent event;
    XNextEvent(dpy, &event);
    if(!XFilterEvent(&event, mainwin))
    {
      switch(event.type)
      {
        case Expose:
          if(!event.xexpose.count)
          {
            if(event.xexpose.window == mainwin) { }//printf("event: exposing\n"); }
          }
        case LeaveNotify:
          //  if(event.xcrossing.window==icon1.iconwin) {
          //     printf("leave\n");
          //  }
          break;
        case EnterNotify:
          //  if(event.xcrossing.window==icon1.iconwin) {
          //     printf("enter\n");
          //   }
          break;
        case ButtonPress:

          for (int i=0;i<dircount;i++)
          {
            if(event.xcrossing.window==icons[i].iconwin)
            {
              if ((event.xbutton.time - last_icon_click) < dblClickTime)
              {
                printf("* double click! *\n");
                icons[i].pmA = icons[i].pm2;
                if (strcmp(icons[i].type,"file")==0)
                {
                  const char *cmd = "DISPLAY=:1 xdg-open";
                  const char *path = icons[i].path;
                  const char *exec = icons[i].name;
                  char *line=alloca(strlen(cmd) + strlen(icons[i].path) + strlen(exec) +2);
                  sprintf(line, "%s %s%s &", cmd, path, exec);
                  printf("line=%s\n",line);
                  system(line);
                }
                else if (strcmp(icons[i].type,"directory")==0)
                {
                  spawn_new_wb(icons[i].path,icons[i].name );
                }
              }
              else
              {
                last_icon_click=event.xbutton.time;
                // toggle active icon
                if (icons[i].pmA == icons[i].pm1) { icons[i].pmA = icons[i].pm2; }
                else if (icons[i].pmA == icons[i].pm2) { icons[i].pmA = icons[i].pm1; }
                printf("simple click!\n");

              }
              // force redraw
              XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
              XClearWindow(dpy, icons[i].iconwin);
              XFlush(dpy);
            }
          }
          break;
        case ButtonRelease:
          // if(event.xcrossing.window==icon1.iconwin)  {
          //   printf("release\n");
          // }
          break;
        case KeyPress:
          break;
        case ConfigureNotify: // resize or move event
          //  printf("resize event: x=%d, y=%d, width=%d, height=%d\n",
          //         event.xconfigure.x,
          //         event.xconfigure.y,
          //         event.xconfigure.width,
          //         event.xconfigure.height);
          win_x=event.xconfigure.x;
          win_y=event.xconfigure.y;
          win_width=event.xconfigure.width;
          win_height=event.xconfigure.height;
          break;
      }
    }
  }
}
