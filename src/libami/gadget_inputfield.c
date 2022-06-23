
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

#define MAX_CMD_CHARS 256
#define VISIBLE_CMD_CHARS 35

#define BOT_SPACE 4
#define TEXT_SIDE 8
#define BUT_SIDE 12
#define TOP_SPACE 4
#define INT_SPACE 7
#define BUT_VSPACE 2
#define BUT_HSPACE 8

static XIM xim = (XIM) NULL;
static XIC xic = (XIC) NULL;

Window strwin;
int strgadw, strgadh, fh, mainw, mainh, butw;
struct DrawInfo dri;
Display *dpy;
GC gc;

char cmdline[MAX_CMD_CHARS+1];
int buf_len=0;
int cur_pos=0;
int left_pos=0;
int cur_x=6;
static int selected=0, depressed=0, stractive=1;


void refresh_str_text()
{
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len>left_pos) 
  {
    int w, c;
    for(l=0; l<buf_len-left_pos; ) 
    {
      c=mbrlen(cmdline+left_pos+l, buf_len-left_pos-l, NULL);
      w=6+XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, l+c);
      if(w>strgadw-6)
        break;
      mx=w;
      l+=c;
    }
    XmbDrawImageString(dpy, strwin, dri.dri_FontSet, gc, 6, 3+dri.dri_Ascent,
                       cmdline+left_pos, l);
  }
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, strwin, gc, mx, 3, strgadw-mx-6, fh);
  if(stractive) 
  {
    if(cur_pos<buf_len) 
    {
      XSetBackground(dpy, gc, ~0);

      l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
      XmbDrawImageString(dpy, strwin, dri.dri_FontSet, gc, cur_x,
                         3+dri.dri_Ascent, cmdline+cur_pos, l);

      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    } 
    else 
    {
      XSetForeground(dpy, gc, ~0);
      XFillRectangle(dpy, strwin, gc, cur_x, 3,
                     XExtentsOfFontSet(dri.dri_FontSet)->
                     max_logical_extent.width, fh);
    }
  }
}

void refresh_str()
{
  XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
  XDrawLine(dpy, strwin, gc, 0, strgadh-1, 0, 0);
  XDrawLine(dpy, strwin, gc, 0, 0, strgadw-2, 0);
  XDrawLine(dpy, strwin, gc, 3, strgadh-2, strgadw-4, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-4, strgadh-2, strgadw-4, 2);
  XDrawLine(dpy, strwin, gc, 1, 1, 1, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-3, 1, strgadw-3, strgadh-2);
  XSetForeground(dpy, gc, dri.dri_Pens[SHADOWPEN]);
  XDrawLine(dpy, strwin, gc, 1, strgadh-1, strgadw-1, strgadh-1);
  XDrawLine(dpy, strwin, gc, strgadw-1, strgadh-1, strgadw-1, 0);
  XDrawLine(dpy, strwin, gc, 3, strgadh-3, 3, 1);
  XDrawLine(dpy, strwin, gc, 3, 1, strgadw-4, 1);
  XDrawLine(dpy, strwin, gc, 2, 1, 2, strgadh-2);
  XDrawLine(dpy, strwin, gc, strgadw-2, 1, strgadw-2, strgadh-2);
}

void strkey(XKeyEvent *e)
{
  void endchoice(void);

  Status stat;

  KeySym ks;
  char buf[256];
  int x, i, n;

  n=XmbLookupString(xic, e, buf, sizeof(buf), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)
  {
    switch(ks) 
    {
      case XK_Return:
      case XK_Linefeed:
        selected=1;
        endchoice();
        break;
      case XK_Left:
        if(cur_pos) 
        {
          int p=cur_pos;
          int z;
          while(p>0) 
          {
            --p;
            if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) 
            {
              cur_pos=p;
              break;
            }
          }
        }
        break;
      case XK_Right:
        if(cur_pos<buf_len) 
        {
          int l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
          if(l>0)
            cur_pos+=l;
        }
        break;
      case XK_Begin:
        cur_pos=0;
        break;
      case XK_End:
        cur_pos=buf_len;
        break;
      case XK_Delete:
        if(cur_pos<buf_len) 
        {
          int l=1;
          l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
          if(l<=0)
            break;
          buf_len-=l;
          for(x=cur_pos; x<buf_len; x++)
            cmdline[x]=cmdline[x+l];
          cmdline[x] = 0;
        } 
        else 
          XBell(dpy, 100);
        break;
      case XK_BackSpace:
        if(cur_pos>0) 
        {
          int l=1;
          int p=cur_pos;
          while(p>0) 
          {
            --p;
            if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) 
            {
              l=cur_pos-p;
              break;
            }
          }
          buf_len-=l;
          for(x=(cur_pos-=l); x<buf_len; x++)
            cmdline[x]=cmdline[x+l];
          cmdline[x] = 0;
        } else XBell(dpy, 100);
        break;
      default:
        if(stat == XLookupBoth)
          stat = XLookupChars;
    }
  }  
  if(stat == XLookupChars) 
  {
    for(i=0; i<n && buf_len<MAX_CMD_CHARS; i++) 
    {
      for(x=buf_len; x>cur_pos; --x)
        cmdline[x]=cmdline[x-1];
      cmdline[cur_pos++]=buf[i];
      buf_len++;
    }
    if(i<n)
      XBell(dpy, 100);
  }
  if(cur_pos<left_pos)
  {
    left_pos=cur_pos;
  }
  cur_x=6;

  if(cur_pos>left_pos)
  {
    cur_x+=XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, cur_pos-left_pos);
  }
  if(cur_pos<buf_len) 
  {
    int l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
    x=XmbTextEscapement(dri.dri_FontSet, cmdline+cur_pos, l);
  } 
  else
  {
    x=XExtentsOfFontSet(dri.dri_FontSet)->max_logical_extent.width;
  }

  if((x+=cur_x-(strgadw-6))>0) 
  {
    cur_x-=x;
    while(x>0) {
      int l=mbrlen(cmdline+left_pos, buf_len-left_pos, NULL);
      x-=XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, l);
      left_pos += l;
    }
    cur_x+=x;
  }
  refresh_str_text();
}

void strbutton(XButtonEvent *e)
{
  int w, l=1;
  stractive=1;
  cur_pos=left_pos;
  cur_x=6;
  while(cur_x<e->x && cur_pos<buf_len) 
  {

    l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
    if(l<=0)
      break;
    w=XmbTextEscapement(dri.dri_FontSet, cmdline+cur_pos, l);

    if(cur_x+w>e->x)
      break;
    cur_x+=w;
    cur_pos+=l;
  }
  refresh_str();
}

int create_inputfield(Window mainwin, int x, int y, int width, int height)
{
  // XWindowAttributes attr;
  // root = RootWindow(dpy, DefaultScreen(dpy));
  // XGetWindowAttributes(dpy, root, &attr);
  // init_dri(&dri, dpy, root, attr.colormap, False);
  char *p; // input xic/xim
  
  strgadw=VISIBLE_CMD_CHARS*XExtentsOfFontSet(dri.dri_FontSet)-> max_logical_extent.width+12;
  strgadh=(fh=dri.dri_Ascent+dri.dri_Descent)+6;
  strwin=XCreateSimpleWindow(dpy, mainwin, x, y, width, height, 0,
                             dri.dri_Pens[SHADOWPEN],
                             dri.dri_Pens[BACKGROUNDPEN]);
  XSelectInput(dpy, strwin, ExposureMask|ButtonPressMask);
  
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
  
  return 0;
}
