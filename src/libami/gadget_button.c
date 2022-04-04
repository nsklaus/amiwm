
#include <X11/Xlib.h>
#include <X11/Xutil.h>
// #include <X11/Xatom.h>
// #include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
//#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <locale.h>
#include <wchar.h>
#include "drawinfo.h"
#include "libami.h"

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

gadget_button *button_create(Display *dpy, struct DrawInfo *dri, GC gc, Window mainwin, int x, int y)
{
  gadget_button *b;
  
  b = calloc(1, sizeof(*b));
  if (b == NULL) {
    return (NULL);
  }
  b->dpy = dpy;
  b->dri = dri;
  b->x = x;
  b->y = y;
  b->butw = 55;
  b->buth = 20;
  b->label = strdup("");
  b->gc = gc;
  
  b->w = XCreateSimpleWindow(dpy, mainwin, x, y, 55,  20,  0, 
                             dri->dri_Pens[SHADOWPEN],
                             dri->dri_Pens[BACKGROUNDPEN]);
  
  XSelectInput(dpy, b->w, ExposureMask | ButtonPressMask | ButtonReleaseMask | 
                          EnterWindowMask | LeaveWindowMask);
  
  return (b);
}

void button_set_text(gadget_button *b, const char *label)
{
  if (b->label != NULL)
    free(b->label);
  b->label = strdup(label);
  int l=strlen(b->label); 
  int tw = XmbTextEscapement(b->dri->dri_FontSet, b->label, l);
  printf("tw=%d\n",tw);
  b->butw=tw+10;
  XResizeWindow(b->dpy,b->w,b->butw,17);

}

void button_refresh(gadget_button *b)
{
  int fh = b->dri->dri_Ascent + b->dri->dri_Descent;
  int h = fh + (2 * BUT_VSPACE);
  int l=strlen(b->label); 
  int tw = XmbTextEscapement(b->dri->dri_FontSet, b->label, l);

  XSetForeground(b->dpy, b->gc, b->dri->dri_Pens[TEXTPEN]);
  
  XmbDrawString(b->dpy, b->w, b->dri->dri_FontSet, b->gc,
               (5), b->dri->dri_Ascent+BUT_VSPACE, b->label, l);

  // top line
  XSetForeground(b->dpy, b->gc, b->dri->dri_Pens[b->depressed ? SHADOWPEN:SHINEPEN]);
  XDrawLine(b->dpy, b->w, b->gc, 0, 0, b->butw-2, 0);
  
  // left line
  XDrawLine(b->dpy, b->w, b->gc, 0, 0, 0, h-2);
  
  // bottom line
  XSetForeground(b->dpy, b->gc, b->dri->dri_Pens[b->depressed ? SHINEPEN:SHADOWPEN]);
  XDrawLine(b->dpy, b->w, b->gc, 1, h-1, b->butw-1, h-1);
  
  // right line
  XDrawLine(b->dpy, b->w, b->gc, b->butw-1, 1, b->butw-1, h-1);
  XSetForeground(b->dpy, b->gc, b->dri->dri_Pens[BACKGROUNDPEN]);
  
  XDrawPoint(b->dpy, b->w, b->gc, b->butw-1, 0);
  XDrawPoint(b->dpy, b->w, b->gc, 0, h-1);
//   Window rac;
//   int x, y, larg, haut, bord, prof;
//   XGetGeometry(b->dpy, b->w, &rac, &x, &y, &larg, &haut, &bord, &prof);
//   printf("button x=%d  y=%d larg=%d  haut=%d\n",x,y,larg,haut);
}

void button_set_depressed(gadget_button *b, int depressed)
{
  b->depressed = depressed;
}

void button_toggle(gadget_button *b)
{
  int pen;
  
  pen = (b->depressed) ? FILLPEN : BACKGROUNDPEN;
  
  
  XSetWindowBackground(b->dpy, b->w, b->dri->dri_Pens[pen]);
  XClearWindow(b->dpy, b->w);
  button_refresh(b);
}

void button_free(gadget_button *b)
{
  XDestroyWindow(b->dpy, b->w);
  free(b->label);
  free(b);
}
