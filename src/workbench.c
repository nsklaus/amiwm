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

static int selected=0, depressed=0, stractive=1;
struct ColorStore colorstore1, colorstore2;

char *progname;
Display *dpy;
Pixmap pm1, pm2;
struct DrawInfo dri;

typedef struct {
  char *name;
  char *path;
  Window iconwin;
  Pixmap pm1;
  Pixmap mp2;
  int x;
  int y;
  int width;
  int height;
} wbicon;
//wbicon icon1;
wbicon *icons;

int dircount;
Window root, mainwin;//, myicon;
int win_x=20, win_y=20, win_width=300, win_height=150;
//int icon_x=10, icon_y=10, icon_width, icon_height;
GC gc;

/** refresh window background */
void refresh_main(void)
{
   for (int i=0; i<dircount;i++){

   }
//   }
//   XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
//   XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, icons[i].x, icons[i].y+35, "some text", strlen("some text"));
//
//   XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);
//   XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, pm1);
  //XCopyArea(dpy, pm1, mainwin, gc, 0, 0, icon_width, icon_height, icon_x, icon_y);
  //}
}

void read_entries() {
  // differentiate between files and directories
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir("/home/klaus/Downloads/");
  while ((dp = readdir(dirp)) != NULL) {
    if (dp->d_type & DT_DIR) {
      // exclude common system entries and (semi)hidden names
      if (dp->d_name[0] != '.'){
        printf ("DIRECTORY: %s\n", dp->d_name);
        dircount++;
      }
    } else
      printf ("FILE: %s\n", dp->d_name);
    }
  closedir(dirp);
  icons = calloc(dircount, sizeof(wbicon));
  //printf("total dircount=%d sizeof(icons)=%lu\n", dircount, sizeof(*icons));

}

void list_entries() {
  unsigned long *iconcolor;
  unsigned long bleh[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };
  iconcolor = bleh;

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

  struct Image *im1 = icon_do->do_Gadget.GadgetRender;
  struct Image *im2 = icon_do->do_Gadget.SelectRender;

  for (int i=0;i<dircount;i++){
  icons[i].width=icon_do->do_Gadget.Width;
  icons[i].height=icon_do->do_Gadget.Height;
  icons[i].x=10 + (i*80);
  icons[i].y=10;

  icons[i].iconwin=XCreateSimpleWindow(dpy, mainwin, icons[i].x, icons[i].y, icons[i].width, icons[i].height, 1,
                             dri.dri_Pens[BACKGROUNDPEN],
                             dri.dri_Pens[BACKGROUNDPEN]);
  pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN],
                        iconcolor, 7, im1, icons[i].width, icons[i].height, &colorstore1);
  pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN],
                        iconcolor, 7, im2, icons[i].width, icons[i].height, &colorstore2);
  XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, pm1);
  XSelectInput(dpy, icons[i].iconwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  }
  FreeDiskObject(icon_do);
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

  read_entries();
  list_entries();

  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  XSync(dpy, False);


  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    //printf("event type = (%d)\n",event.type);
    if(!XFilterEvent(&event, mainwin)){
     // && !XFilterEvent(&event, icon1.iconwin) ) {

      switch(event.type) {
        case Expose:
          if(!event.xexpose.count) {
            if(event.xexpose.window == mainwin) {
              refresh_main();
            }
          }
        case LeaveNotify:
//           if(event.xcrossing.window==icon1.iconwin) {
//             printf("leave\n");
//           }
          break;
        case EnterNotify:

//           if(event.xcrossing.window==icon1.iconwin) {
//             printf("enter\n");
//           }
          break;
        case ButtonPress:
//           if(event.xcrossing.window==icon1.iconwin) {
//             printf("press\n");
//           }
          break;
        case ButtonRelease:
//           if(event.xcrossing.window==icon1.iconwin) {
//             printf("release\n");
//           }
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
