
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "drawinfo.h"
#include "libami.h"

#ifdef AMIGAOS
#include <pragmas/xlib_pragmas.h>
extern struct Library *XLibBase;
#endif

#define ABOUT_STRING "Global menu test program"

Display *dpy;
Window root, mainwin;
char *progname;
GC gc;
Atom amiwm_menu = None, wm_protocols = None, wm_delete = None;

struct DrawInfo dri;
unsigned long rgbpens[3];

static void SetColor(unsigned n)
{
  if (n<3) {
    XSetWindowBackground(dpy, mainwin, rgbpens[n]);
    XClearWindow(dpy, mainwin);
  }
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

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  XColor screen, exact;
  
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
            XDisplayName(NULL));
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);
  XAllocNamedColor(dpy, attr.colormap, "red", &screen, &exact);
  rgbpens[0] = screen.pixel;
  XAllocNamedColor(dpy, attr.colormap, "green", &screen, &exact);
  rgbpens[1] = screen.pixel;
  XAllocNamedColor(dpy, attr.colormap, "blue", &screen, &exact);
  rgbpens[2] = screen.pixel;
  
  mainwin=XCreateSimpleWindow(dpy, root, 100, 100, 100, 100, 1,
                              dri.dri_Pens[SHADOWPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);
  SetProtocols(mainwin);
  SetMenu(mainwin);
  gc=XCreateGC(dpy, mainwin, 0, NULL);
  size_hints.flags = PResizeInc;
  txtprop1.value=(unsigned char *)"MenuTest";
  txtprop2.value=(unsigned char *)"MenuTest";
  txtprop2.encoding=txtprop1.encoding=XA_STRING;
  txtprop2.format=txtprop1.format=8;
  txtprop1.nitems=strlen((char *)txtprop1.value);
  txtprop2.nitems=strlen((char *)txtprop2.value);
  XSetWMProperties(dpy, mainwin, &txtprop1, &txtprop2, argv, argc,
                   &size_hints, NULL, NULL);
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    switch(event.type) {
      case ClientMessage:
        if (event.xclient.message_type == amiwm_menu &&
          event.xclient.format == 32) {
          unsigned int menunum = event.xclient.data.l[0];
        unsigned menu = menunum & 0x1f;
        unsigned item = (menunum >> 5) & 0x3f;
        unsigned subitem = (menunum >> 11) & 0x1f;
        switch (menu) {
          case 0:
            switch (item) {
              case 0:
                SetColor(subitem);
                break;
              case 1:
                goto quit;
            }
            break;
              case 1:
                switch (item) {
                  case 0:
                    #ifdef AMIGAOS
                    system("RUN <>NIL: requestchoice menutest \"" ABOUT_STRING "\" Ok");
                    #else
                    (void)! system("requestchoice >/dev/null menutest \"" ABOUT_STRING "\" Ok &");
                    #endif
                    break;
                }
                break;
        }
          } else if (event.xclient.message_type == wm_protocols &&
            event.xclient.format == 32 &&
            event.xclient.data.l[0] == wm_delete) {
            goto quit;
            }
            break;
    }
  }
  quit:
  XFreeColors(dpy, attr.colormap, rgbpens, 3, 0);
  term_dri(&dri, dpy, attr.colormap);
  return 0;
}
