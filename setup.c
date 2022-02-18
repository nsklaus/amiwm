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

#include "client.h"

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

static const char ok_txt[]="Save", cancel_txt[]="Cancel";
static const char enter_txt[]="Enter Command and its Arguments:";
static const char cmd_txt[]="Command:";
static const char FastQuit_txt[]="FastQuit:";
static const char SizeBorder_txt[]="SizeBorder:";
static const char IconPalette_txt[]="IconPalette:";
static const char IconDir_txt[]="IconDir:";
static const char CustomIconsOnly_txt[]="CustomIconsOnly:";
static const char ShortLabelIcons_txt[]="ShortLabelIcons:";
static char choice_txt[]="yes";


XContext client_context, screen_context, icon_context;

static int selected=0, depressed=0, stractive=1;
static Window button[8];
static const char *buttxt[]={ NULL, ok_txt, cancel_txt, choice_txt };

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

static XIM xim = (XIM) NULL;
static XIC xic = (XIC) NULL;

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
//   XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
//   XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, TEXT_SIDE,
//                 TOP_SPACE+dri.dri_Ascent, enter_txt, strlen(enter_txt));
  XSetForeground(dpy, gc, dri.dri_Pens[HIGHLIGHTTEXTPEN]);

  w=XmbTextEscapement(dri.dri_FontSet, FastQuit_txt, strlen(FastQuit_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 30, FastQuit_txt, strlen(FastQuit_txt));

  w=XmbTextEscapement(dri.dri_FontSet, SizeBorder_txt, strlen(SizeBorder_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 60, SizeBorder_txt, strlen(SizeBorder_txt));

  w=XmbTextEscapement(dri.dri_FontSet, IconPalette_txt, strlen(IconPalette_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 90, IconPalette_txt, strlen(IconPalette_txt));

  w=XmbTextEscapement(dri.dri_FontSet, IconDir_txt, strlen(IconDir_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 120, IconDir_txt, strlen(IconDir_txt));

  w=XmbTextEscapement(dri.dri_FontSet, CustomIconsOnly_txt, strlen(CustomIconsOnly_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 150, CustomIconsOnly_txt, strlen(CustomIconsOnly_txt));

  w=XmbTextEscapement(dri.dri_FontSet, ShortLabelIcons_txt, strlen(ShortLabelIcons_txt));
  XmbDrawString(dpy, mainwin, dri.dri_FontSet, gc, 20, 180, ShortLabelIcons_txt, strlen(ShortLabelIcons_txt));

}

/** refresh text string in input field (typing) */
extern size_t mbrlen(); // define proto mbrlen() to silence ccompile warning
void refresh_str_text(void)
{
  int l, mx=6;
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  if(buf_len>left_pos) {
    int w, c;
    for(l=0; l<buf_len-left_pos; ) {
      c=mbrlen(cmdline+left_pos+l, buf_len-left_pos-l, NULL);
      w=6+XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, l+c);
      if(w>strgadw-6)
        break;
      mx=w;
      l+=c;
    }
    XmbDrawImageString(dpy, strwin, dri.dri_FontSet,
                       gc, 6, 3+dri.dri_Ascent,
                       cmdline+left_pos, l);
  }
  XSetForeground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
  XFillRectangle(dpy, strwin, gc, mx, 3, strgadw-mx-6, fh);
  if(stractive) {
    if(cur_pos<buf_len) {
      XSetBackground(dpy, gc, ~0);

      l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
      XmbDrawImageString(dpy, strwin, dri.dri_FontSet,
                         gc, cur_x, 3+dri.dri_Ascent,
                         cmdline+cur_pos, l);
      XSetBackground(dpy, gc, dri.dri_Pens[BACKGROUNDPEN]);
    }
    else {
      XSetForeground(dpy, gc, ~0);
      XFillRectangle(dpy, strwin, gc, cur_x, 3,
                     XExtentsOfFontSet(dri.dri_FontSet)->
                     max_logical_extent.width, fh);
    }
  }
}

/** refresh drawing of text field borders */
void refresh_str(void)
{
  refresh_str_text();
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


/** work with text string entered */
void strkey(XKeyEvent *e)
{
  void endchoice(void);
  Status stat;
  KeySym ks;
  char buf[256];
  int x, i, n;
  n=XmbLookupString(xic, e, buf, sizeof(buf), &ks, &stat);
  if(stat == XLookupKeySym || stat == XLookupBoth)

  switch(ks) {
    case XK_Return:
    case XK_Linefeed:
      selected=1;
      endchoice();
      break;
    case XK_Left:
      if(cur_pos) {
        int p=cur_pos;
        while(p>0) {
          --p;
          if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) {
            cur_pos=p;
            break;
          }
        }
      }
      break;
    case XK_Right:
      if(cur_pos<buf_len) {
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
      if(cur_pos<buf_len) {
        int l=1;
        l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
        if(l<=0)
          break;
        buf_len-=l;
        for(x=cur_pos; x<buf_len; x++){
          cmdline[x]=cmdline[x+l];
        }
        cmdline[x] = 0;
      }
      else {
        XBell(dpy, 100);
      }
      break;
    case XK_BackSpace:
      if(cur_pos>0) {
        int l=1;
        int p=cur_pos;
        while(p>0) {
          --p;
          if(((int)mbrlen(cmdline+p, cur_pos-p, NULL))>0) {
          l=cur_pos-p;
          break;
        }
      }
      buf_len-=l;
      for(x=(cur_pos-=l); x<buf_len; x++) {
        cmdline[x]=cmdline[x+l];
      }
      cmdline[x] = 0;
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
    for(i=0; i<n && buf_len<MAX_CMD_CHARS; i++) {
      for(x=buf_len; x>cur_pos; --x)
        cmdline[x]=cmdline[x-1];
      cmdline[cur_pos++]=buf[i];
      buf_len++;
    }
    if(i<n)
      XBell(dpy, 100);
  }
  if(cur_pos<left_pos)
    left_pos=cur_pos;
  cur_x=6;

  if(cur_pos>left_pos)
    cur_x+=XmbTextEscapement(dri.dri_FontSet, cmdline+left_pos, cur_pos-left_pos);
  if(cur_pos<buf_len) {
    int l=mbrlen(cmdline+cur_pos, buf_len-cur_pos, NULL);
    x=XmbTextEscapement(dri.dri_FontSet, cmdline+cur_pos, l);
  } else
    x=XExtentsOfFontSet(dri.dri_FontSet)->max_logical_extent.width;

  if((x+=cur_x-(strgadw-6))>0) {
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

/** what is this for */
void strbutton(XButtonEvent *e)
{

  int w, l=1;
  stractive=1;
  cur_pos=left_pos;
  cur_x=6;
  while(cur_x<e->x && cur_pos<buf_len) {

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

void endchoice()
{
  int c=selected;
  abortchoice();
  XCloseDisplay(dpy);
  if(c==1){
    printf("ss_cmdline=%s\n", cmdline);
    system(cmdline);
  }
  exit(0);
}

int main(int argc, char *argv[])
{
  XWindowAttributes attr;
  static XSizeHints size_hints;
  static XTextProperty txtprop1, txtprop2;
  // clickable buttons
  Window b_fastq, b_sizer, b_palette, b_customicon, b_shortlabel, b_ok, b_cancel;
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
  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  // width and height of input field borders
  strgadw=100;
  strgadh=(fh=dri.dri_Ascent+dri.dri_Descent)+6;

  butw=XmbTextEscapement(dri.dri_FontSet, ok_txt, strlen(ok_txt))+2*BUT_HSPACE;
  w2=XmbTextEscapement(dri.dri_FontSet, cancel_txt, strlen(cancel_txt))+2*BUT_HSPACE;
  if(w2>butw)
    butw=w2;

  mainw=2*(BUT_SIDE+butw)+BUT_SIDE;
  w2=XmbTextEscapement(dri.dri_FontSet, enter_txt, strlen(enter_txt))+2*TEXT_SIDE;
  if(w2>mainw)
    mainw=w2;

  w2=strgadw+XmbTextEscapement(dri.dri_FontSet, cmd_txt, strlen(cmd_txt))+
    2*TEXT_SIDE+2*BUT_SIDE+butw;
  if(w2>mainw)
    mainw=w2;

  mainh=3*fh+TOP_SPACE+BOT_SPACE+2*INT_SPACE+2*BUT_VSPACE;

  mainwin=XCreateSimpleWindow(dpy, root, mainx, mainy, mainw, 300, 1,
                              dri.dri_Pens[SHADOWPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  strwin=XCreateSimpleWindow(dpy, mainwin, 150, 105, strgadw, strgadh, 0,
                             dri.dri_Pens[SHADOWPEN],
                             dri.dri_Pens[BACKGROUNDPEN]);

  b_fastq=XCreateSimpleWindow(dpy, mainwin, 150,15,butw, fh+2*BUT_VSPACE, 0,
                              dri.dri_Pens[SHADOWPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  b_sizer=XCreateSimpleWindow(dpy, mainwin, 150,45,butw, fh+2*BUT_VSPACE, 0,
                              dri.dri_Pens[SHADOWPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  b_palette =XCreateSimpleWindow(dpy, mainwin, 150,75,butw, fh+2*BUT_VSPACE, 0,
                                 dri.dri_Pens[SHADOWPEN],
                                 dri.dri_Pens[BACKGROUNDPEN]);

  b_customicon=XCreateSimpleWindow(dpy, mainwin, 150,135,butw, fh+2*BUT_VSPACE, 0,
                                   dri.dri_Pens[SHADOWPEN],
                                   dri.dri_Pens[BACKGROUNDPEN]);

  b_shortlabel=XCreateSimpleWindow(dpy, mainwin, 150,165,butw, fh+2*BUT_VSPACE, 0,
                                   dri.dri_Pens[SHADOWPEN],
                                   dri.dri_Pens[BACKGROUNDPEN]);

  b_ok=XCreateSimpleWindow(dpy, mainwin, BUT_SIDE,
                           mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                           butw, fh+2*BUT_VSPACE, 0,
                           dri.dri_Pens[SHADOWPEN],
                           dri.dri_Pens[BACKGROUNDPEN]);

  b_cancel=XCreateSimpleWindow(dpy, mainwin, mainw-butw-BUT_SIDE,
                               mainh-BOT_SPACE-2*BUT_VSPACE-fh,
                               butw, fh+2*BUT_VSPACE, 0,
                               dri.dri_Pens[SHADOWPEN],
                               dri.dri_Pens[BACKGROUNDPEN]);
  button[0]=None;
  button[1]=b_ok;
  button[2]=b_cancel;
  button[3]=b_fastq;
  button[4]=b_sizer;
  button[5]=b_palette;
  button[6]=b_shortlabel;
  button[7]=b_customicon;
  //totalbuttons = sizeof button / sizeof button[0];
  printf("totalbuttonlength=%d\n",(int)(sizeof button / sizeof button[0]));
  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask);
  XSelectInput(dpy, strwin, ExposureMask|StructureNotifyMask|ButtonPressMask);
  XSelectInput(dpy, b_fastq, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_sizer, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_palette, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_shortlabel, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_customicon, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
  XSelectInput(dpy, b_ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|EnterWindowMask|LeaveWindowMask);
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


  size_hints.flags = USSize;
  txtprop1.value=(unsigned char *)"Setup";
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
              //printf("expose event %d, winh= \n", bleh+=1 );
              refresh_main();
            }
            else if(event.xexpose.window == strwin) {
              refresh_str();
            }
            else if(event.xexpose.window == b_fastq) {
              refresh_button(b_fastq, choice_txt, 3);
            }
            else if(event.xexpose.window == b_sizer) {
              refresh_button(b_sizer, "bottom", 4);
            }
            else if(event.xexpose.window == b_palette) {
              refresh_button(b_palette, "magicwb", 5);
            }
            else if(event.xexpose.window == b_shortlabel) {
              refresh_button(b_shortlabel, "yes", 6);
            }
            else if(event.xexpose.window == b_customicon) {
              refresh_button(b_customicon, "yes", 7);
            }
            else if(event.xexpose.window == b_ok) {
              refresh_button(b_ok, ok_txt, 1);
            }
            else if(event.xexpose.window == b_cancel) {
              refresh_button(b_cancel, cancel_txt, 2);
            }
          }
        case ConfigureNotify:
          if(event.xconfigure.window == mainwin) {
            // make save and cancel button to stay at bottom of window when it gets resized
            XMoveWindow(dpy, b_ok, 10, event.xconfigure.height - 20);
            XMoveWindow(dpy, b_cancel, event.xconfigure.width - 65, event.xconfigure.height - 20);
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
            if(stractive && event.xbutton.window!=strwin) {
              stractive=0;
              refresh_str();
            }
            if((c=getchoice(event.xbutton.window))) {
              abortchoice();
              depressed=1;
              printf("buttonpress_getchoice=%d\n\n",c);
              toggle(selected=c);
              //printf("selected=%d\n",c);
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
