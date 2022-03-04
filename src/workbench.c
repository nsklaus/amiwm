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
#include "screen.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define MAX_CMD_CHARS 256
#define VISIBLE_CMD_CHARS 35

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

static int selected=0, depressed=0, stractive=1;
struct ColorStore colorstore1, colorstore2;
// struct launcher {
//   struct ColorStore colorstore1, colorstore2;
//   char cmdline[1];
// };
char cmdline[MAX_CMD_CHARS+1];
int buf_len=0;
int cur_pos=0;
int left_pos=0;
int cur_x=6;

char *progname;

Display *dpy;
Pixmap pm1, pm2;
Icon icon1;
int icon_x=10, icon_y=10, icon_width, icon_height;
struct DrawInfo dri;

typedef struct {
  char *name;
  Pixmap pm1;
  Pixmap mp2;
} wbicon;

Window root, mainwin;
int win_x=20, win_y=20, win_width=300, win_height=150;
GC gc;

int strgadw, strgadh, fh, mainw, mainh, butw;
static Window button[3];

/** get button number/id */
int getchoice(Window w)
{
  int i;
  int totalbuttons = (int)(sizeof button / sizeof button[0]);
  for(i=1; i<totalbuttons; i++)
    if(button[i]==w)
      return i;
  return 0;
}

/** refresh window background */
void refresh_main(void)
{
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, icon_x,
                icon_y+35, "some text", strlen("some text"));

  XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);
  XCopyArea(dpy, pm1, mainwin, gc, 0, 0, icon_width, icon_height, icon_x, icon_y);
  //XCopyPlane(dpy, pm, mainwin, gc, 0, 0, width, height, 50, 50, 8);
}


int main(int argc, char *argv[])
{

  /*
  // differentiate between files and directories
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir("/home/klaus/Downloads/");
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_DIR)
    {
      // exclude common system entries and (semi)hidden names
      if (dp->d_name[0] != '.')
        printf ("DIRECTORY: %s\n", dp->d_name);
    } else
      printf ("FILE: %s\n", dp->d_name);
  }
  closedir(dirp);
  */

  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }

  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  mainwin=XCreateSimpleWindow(dpy, root, win_x, win_y, win_width, win_height, 1,
			      dri.dri_Pens[SHADOWPEN],
			      dri.dri_Pens[BACKGROUNDPEN]);
  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);

  size_hints.flags = USSize;
  txtprop1.value=(unsigned char *)"workbench";
  txtprop2.value=(unsigned char *)"ExecuteCmd";
  txtprop2.encoding=txtprop1.encoding=XA_STRING;
  txtprop2.format=txtprop1.format=8;
  txtprop1.nitems=strlen((char *)txtprop1.value);
  txtprop2.nitems=strlen((char *)txtprop2.value);
  XSetWMProperties(dpy, mainwin,
                  &txtprop1,
                  &txtprop2, argv, argc,
                  &size_hints, NULL, NULL);

  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);

  // begin -- display icon in wb window
  struct DiskObject *icon_do = NULL;
  char *icondir="/usr/local/lib/amiwm/icons";
  char *icon="def_drawer.info";
  if (icon != NULL && *icon != 0) {
    int rl=strlen(icon)+strlen(icondir)+2;
    char *fn=alloca(rl);
    sprintf(fn, "%s/%s", icondir, icon);
    fn[strlen(fn)-5]=0;
    printf("fn=%s\n",fn);
    icon_do = GetDiskObject(fn);
  }

  icon_width=icon_do->do_Gadget.Width;
  icon_height=icon_do->do_Gadget.Height;
  unsigned long *iconcolor;
  unsigned long bleh[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };
  iconcolor = bleh;
  struct Image *im1 = icon_do->do_Gadget.GadgetRender;
  struct Image *im2 = icon_do->do_Gadget.SelectRender;

  pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN],
                        iconcolor, 7, im1, icon_width, icon_height, &colorstore1);
  pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN],
                        iconcolor, 7, im2, icon_width, icon_height, &colorstore2);

  XCopyArea(dpy, pm1, mainwin, gc, 0, 0, icon_width, icon_height, icon_x, icon_y);

  XSync(dpy, False);
  FreeDiskObject(icon_do);
  // end -- display icon in wb window

  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    if(!XFilterEvent(&event, mainwin)) {
      switch(event.type) {
        case Expose:
          if(!event.xexpose.count) {
            if(event.xexpose.window == mainwin) {
              refresh_main();
            }
          }
        case LeaveNotify:
          printf("leave\n");
          break;
        case EnterNotify:
          printf("enter\n");
          break;
        case ButtonPress:
          printf("press\n");
          break;
        case ButtonRelease:
          printf("release\n");
          break;
        case KeyPress:
          break;
        case ConfigureNotify: // resize or move event
          printf("resize event: x=%d, y=%d, width=%d, height=%d\n",
                 event.xconfigure.x,
                 event.xconfigure.y,
                 event.xconfigure.width,
                 event.xconfigure.height);
          win_x=event.xconfigure.x;
          win_y=event.xconfigure.y;
          win_width=event.xconfigure.width;
          win_height=event.xconfigure.height;
          break;
      }
    }
  }
}
