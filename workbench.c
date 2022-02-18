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

char cmdline[MAX_CMD_CHARS+1];
int buf_len=0;
int cur_pos=0;
int left_pos=0;
int cur_x=6;

char *progname;

Display *dpy;

struct DrawInfo dri;

Window root, mainwin, strwin;
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

/** refresh buttons in window */
void refresh_button(Window w, const char *txt, int idx)
{
}

/** refresh window background */
void refresh_main(void)
{
  int w;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, TEXT_SIDE,
                TOP_SPACE+dri.dri_Ascent, "some text", strlen("some text"));

  XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);
}

/** refresh text string in input field (typing) */
void refresh_str_text(void)
{
}

/** refresh drawing of text field borders */
void refresh_str(void)
{
}

void strkey(XKeyEvent *e)
{
}

/** stuff */
void strbutton(XButtonEvent *e)
{
}

void toggle(int c)
{
}

void abortchoice()
{
}

void endchoice()
{
}

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  Window ok, cancel;
  int w2, c;
  char *p;
  progname=argv[0];
  if(!(dpy = XOpenDisplay(NULL))) {
    fprintf(stderr, "%s: cannot connect to X server %s\n", progname,
	    XDisplayName(NULL));
    exit(1);
  }
  root = RootWindow(dpy, DefaultScreen(dpy));
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  mainwin=XCreateSimpleWindow(dpy, root, 20, 20, 300, 150, 1,
			      dri.dri_Pens[SHADOWPEN],
			      dri.dri_Pens[BACKGROUNDPEN]);
  strwin=XCreateSimpleWindow(dpy, mainwin, 20,20, 300, 150, 0,
			     dri.dri_Pens[SHADOWPEN],
			     dri.dri_Pens[BACKGROUNDPEN]);
  XSelectInput(dpy, mainwin, ExposureMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, strwin, ExposureMask|StructureNotifyMask|ButtonPressMask);
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
  for(;;) {
    XEvent event;
    XNextEvent(dpy, &event);
    //#ifdef USE_FONTSETS
    if(!XFilterEvent(&event, mainwin)) {
    //#endif
      switch(event.type) {
        case Expose:
          if(!event.xexpose.count) {
            if(event.xexpose.window == mainwin) {
              refresh_main();
            }
            else if(event.xexpose.window == strwin) {
              refresh_str();
            }
          }
        case LeaveNotify:
          if(depressed &&
            event.xcrossing.window==button[selected]) {
            depressed=0;
            toggle(selected);
          }
          break;
        case EnterNotify:
          if((!depressed) && selected &&
            event.xcrossing.window==button[selected]) {
            depressed=1;
            toggle(selected);
          }
          break;
        case ButtonPress:
          if(event.xbutton.button==Button1) {
            if(stractive && event.xbutton.window!=strwin) {
              stractive=0;
              refresh_str();
            }
            if((c=getchoice(event.xbutton.window))) {
              abortchoice();
              depressed=1;
              toggle(selected=c);
            }
            else if(event.xbutton.window==strwin) {
              strbutton(&event.xbutton);
            }
          }
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
          if(stractive) { strkey(&event.xkey); }
      }
    }
  }
}
