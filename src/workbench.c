#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include <sys/types.h>
#include <sys/dir.h>
#include "libami.h"
#include "drawinfo.h"
#include "icon.h"

#define _GNU_SOURCE    // includes _BSD_SOURCE for DT_UNKNOWN etc.

char *progname;
char *icondir = "/usr/local/lib/amiwm/icons";

Display *dpy;
XContext wbcontext;
struct DrawInfo dri;
static Window win;

Window root, mainwin, strwin;
GC gc;

struct DiskObject *icon_do = NULL;

struct launcher {
  struct ColorStore colorstore1, colorstore2;
  char cmdline[1];
};

/** refresh window background */
void refresh_main(void)
{
  /*
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 5, 20, "some text", strlen("some text"));
  XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);
  */
}

int main(int argc, char *argv[])
{
  wbcontext = XUniqueContext();
  // test differentiate between files and directories
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir("/home/klaus/Downloads/");
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_DIR)
    {
      /* exclude common system entries and (semi)hidden names */
      if (dp->d_name[0] != '.')
        printf ("DIRECTORY: %s\n", dp->d_name);
    } else
        printf ("FILE: %s\n", dp->d_name);
  }
  closedir(dirp);


  icondir = "/usr/local/lib/amiwm/icons";
  //Window win;
  char *cmdline="/usr/bin/xterm";
  char *icon = "def_tool.info";
  struct launcher *l = malloc(sizeof(struct launcher)+strlen(cmdline));


  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;

  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "cannot connect to X server %s\n", XDisplayName(NULL));
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  mainwin=XCreateSimpleWindow(dpy, root, 20, 20, 300, 150, 1,
			      dri.dri_Pens[SHADOWPEN],
			      dri.dri_Pens[BACKGROUNDPEN]);

  win=XCreateSimpleWindow(dpy, mainwin, 30, 30, 50, 50, 0,
                             dri.dri_Pens[SHADOWPEN],
                             dri.dri_Pens[BACKGROUNDPEN]);

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


  if (l == NULL) {
    fprintf(stderr, "%s: out of memory!\n", progname);
    exit(1);
  }

  memset(l, 0, sizeof(*l));
  strcpy(l->cmdline, cmdline);

  if (icon != NULL && *icon != 0) {

    int rl=strlen(icon)+strlen(icondir)+2;
    char *fn=alloca(rl);
    sprintf(fn, "%s/%s", icondir, icon);

    fn[strlen(fn)-5]=0;
    icon_do = GetDiskObject(fn);
  }

  if(icon_do == NULL)
    icon_do = GetDefDiskObject(3/*WBTOOL*/);

  if(icon_do == NULL) {
    md_errormsg(md_root, "Failed to load icon");
    md_exit(0);
  }

  Pixmap icon_icon1, icon_icon2;
//   icon_icon1 =
//   md_image_to_pixmap(win, dri.dri_Pens[BACKGROUNDPEN],
//                      (struct Image *)icon_do->do_Gadget.GadgetRender,
//                      icon_do->do_Gadget.Width, icon_do->do_Gadget.Height,
//                      &l->colorstore1);


  int width = icon_do->do_Gadget.Width;
  int height = icon_do->do_Gadget.Height;
  icon_icon2 =
  image_to_pixmap(dpy,win,gc,dri.dri_Pens[BACKGROUNDPEN], 0, 0,
                  (struct Image *)icon_do->do_Gadget.GadgetRender,
                  icon_do->do_Gadget.Width, icon_do->do_Gadget.Height,
                  &l->colorstore1);

  XSetWindowBackgroundPixmap(dpy,win,icon_icon2);
  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, win, ExposureMask|StructureNotifyMask|ButtonPressMask);
  //XSetWindowBackgroundPixmap(dpy, mainwin, icon_icon1);
  //win = md_create_appicon(mainwin, 0, 0, "grmbl", icon_icon1, icon_icon1, None);
  //XSaveContext(dpy, mainwin, wbcontext, (XPointer)l);

  //XCopyPlane(dpy, icon_icon2, mainwin, gc, 0, 0, width, height, 5, 5, 1);
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);



  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    if(!XFilterEvent(&event, mainwin)) {
      switch(event.type) {
        case Expose:
          if (event.xany.window == mainwin)
            printf("\nbleh\n");
          if(!event.xexpose.count) {
            if(event.xexpose.window == mainwin) {
              //printf("\nexpose mainwin\n");
              //refresh_main();
            }
            else if(event.xexpose.window == mainwin)
              //printf("\nexpose win\n");
              continue;
          }
        case LeaveNotify:
          if (event.xcrossing.window==win)
            //printf("\n win: leave notify\n");
          break;
        case EnterNotify:
          if (event.xcrossing.window==win)
            //printf("\n win: enter notify\n");
          break;
        case ButtonPress:
          break;
        case ButtonRelease:
          break;
        case KeyPress:
          break;
      }
    }
    if(!XFilterEvent(&event, win)) {
      switch(event.type) {
        case LeaveNotify:
          break;
        case EnterNotify:
//           if((!depressed) && selected &&
//             event.xcrossing.window==button[selected]) {}
          //printf("\nenter notify\n");
          break;
      }
    }
  }
}
