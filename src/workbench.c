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

#include <unistd.h>

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

int dblClickTime=400;
Time last_icon_click=0, last_double=0;

static int selected=0, depressed=0, stractive=1;
struct ColorStore colorstore1, colorstore2;

char *progname;
Display *dpy;
char *viewmode="icons";

struct DrawInfo dri;
Pixmap pm1, pm2;

typedef struct
{
  char *name;
  char *path;
  char *type;   // file or dir
  Window iconwin;
  Pixmap pm1;   // unselected icon
  Pixmap pm2;   // selected icon
  Pixmap pm3;   // unselected label
  Pixmap pm4;   // selected label
  Pixmap pmA;   // active pixmap

  int p_width;  // store icon mode dimensions (pixmap)
  int p_height;
  int t_width;  // store list mode dimensions (text)
  int t_height;
  int x;
  int y;
  int width;    // active dimensions
  int height;
  Bool selected;
  Bool dragging;
} wbicon;

wbicon *icons;
Window ww; //xreparent and xtranslatecoordinates
wbicon icon_temp; // copy of selected icon
XWindowAttributes xwa;

int dircount;
Window root, mainwin;//, myicon;
int win_x=20, win_y=20, win_width=300, win_height=150;
GC gc;

void build_icons();
void event_loop();

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

char * get_viewmode()
{
  return viewmode;
}

void reset_view()
{
  for (int i=0;i<dircount;i++)
  {
//     XFreePixmap(dpy, icons[i].pm1);
//     XFreePixmap(dpy, icons[i].pm2);
//     XFreePixmap(dpy, icons[i].pmA);
    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, None);
    XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[BACKGROUNDPEN]);
    XClearWindow(dpy, icons[i].iconwin);
  }
  XFlush(dpy);
}

void list_entries()
{
  viewmode="list";
  printf("VIEWMODE now =%s\n",get_viewmode());

  for (int i=0;i<dircount;i++)
  {
    icons[i].width  = icons[i].t_width;
    icons[i].height = icons[i].t_height;
    icons[i].x = 10;
    icons[i].y = 10 + i*16;
    icons[i].pmA = icons[i].pm3;
    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pm3);
    XResizeWindow(dpy,icons[i].iconwin,icons[i].width,icons[i].height);
    //XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[FILLPEN]);


//     if (strcmp(icons[i].type,"directory")==0)
//     {
//       XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[FILLPEN]);
//       XSetBackground(dpy,gc,dri.dri_Pens[FILLPEN]);
//       XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
//       XDrawImageString(dpy, icons[i].iconwin, gc, 5, 12, icons[i].name, strlen(icons[i].name));
//     }
//     else if (strcmp(icons[i].type,"file")==0)
//     {
//       XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[SHADOWPEN]);
//       XSetBackground(dpy,gc,dri.dri_Pens[SHADOWPEN]);
//       XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
//       XDrawImageString(dpy, icons[i].iconwin, gc, 5, 12, icons[i].name, strlen(icons[i].name));
//     }

    XMoveWindow(dpy,icons[i].iconwin,icons[i].x,icons[i].y);
    XClearWindow(dpy, icons[i].iconwin);
  }
  XFlush(dpy);
}

void list_entries_icons()
{
  viewmode="icons";
  printf("VIEWMODE now=%s\n",get_viewmode());
  //build_icons();
  int newline_x = 0;
  int newline_y = 0;

   for (int i=0;i<dircount;i++)
   {

    if(newline_x*80 < win_width)
    {
      icons[i].x=10 + (newline_x*80);
      icons[i].y=10 + (newline_y*50);
      newline_x++;
    }
    else if(newline_x*80+50 > win_width)
    {
      newline_x = 0;
      newline_y++;
      icons[i].x=10 + (newline_x*80);
      icons[i].y=10 + (newline_y*50);
      newline_x++;
    }
    icons[i].width  = icons[i].p_width;
    icons[i].height = icons[i].p_height;
    XMoveWindow(dpy,icons[i].iconwin,icons[i].x,icons[i].y);
    XResizeWindow(dpy,icons[i].iconwin,icons[i].width+18,icons[i].height+15);
    icons[i].pmA = icons[i].pm1;
    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
    XClearWindow(dpy, icons[i].iconwin);
    XFlush(dpy);
  }
}

void build_icons()
{
  printf("passing through build icons\n");
  // icon palette
  unsigned long iconcolor[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };
  struct DiskObject *icon_do = NULL;
  char *icondir="/usr/local/lib/amiwm/icons";
  char *icon = "def_drawer.info";//="def_tool.info";
  XSetWindowAttributes xswa;
  xswa.override_redirect = True;
  for (int i=0;i<dircount;i++)
  {
    icons[i].iconwin = XCreateWindow( dpy,mainwin, icons[i].x, icons[i].y, icons[i].width+18, icons[i].height+15, 1, 24, InputOutput, CopyFromParent, CWBackPixel|CWOverrideRedirect, &xswa);

    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
    XSelectInput(dpy, icons[i].iconwin, ExposureMask|CWOverrideRedirect|KeyPressMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask);

    int label_width = XmbTextEscapement(dri.dri_FontSet, icons[i].name, strlen(icons[i].name));
    icons[i].t_width  = label_width+10;
    icons[i].t_height = 15;

    if(strcmp(icons[i].type, "directory") == 0) { icon="def_drawer.info"; }
    else if(strcmp(icons[i].type, "file") == 0) { icon="def_tool.info"; }

    /*
    char *buf = icons[i].name;
    char * ptr;
    int    ch = '.';
    ptr = strrchr( buf, ch );
    if (ptr !=NULL)
    {
      if ( strcmp(ptr, ".info") == 0 )
      {
        printf("my_result=%s\n",icons[i].name);
        icondir=icons[i].path;
        icon=icons[i].name;

      }
      else {
        icondir="/usr/local/lib/amiwm/icons";
      }
    }
*/

    //printf("path= %s icon=%s\n",icondir, icon);
/*
    char *fname = icons[i].name;
    char *buf1 = alloca(strlen(fname)+6);
    sprintf(buf1, "%s.info", fname);
    printf("buf=%s\n",buf1);

    if( access( icons[i].name, F_OK ) == 0 ) {
      printf("file exists, name=%s\n",icons[i].name);
      icon = icons[i].name;
    }
    else
    {*/
//       if(strcmp(icons[i].type, "directory") == 0) { icon="def_drawer.info"; }
//       else if(strcmp(icons[i].type, "file") == 0) { icon="def_tool.info"; }
//     }
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

    icons[i].p_width  = icon_do->do_Gadget.Width;
    icons[i].p_height = icon_do->do_Gadget.Height;
    icons[i].width  = icons[i].p_width;
    icons[i].height = icons[i].p_height;

    pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                          im1, icons[i].width, icons[i].height, &colorstore1);
    pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                          im2, icons[i].width, icons[i].height, &colorstore2);

    // icon mode
    icons[i].pm1 = XCreatePixmap(dpy, pm1, icons[i].width+18, icons[i].height+15, 24);
    XFillRectangle(dpy,icons[i].pm1, gc, 0,0,icons[i].width+18, icons[i].height+15);
    XCopyArea(dpy, pm1, icons[i].pm1, gc, -9, 0, icons[i].width+18, icons[i].height, 0, 0);

    icons[i].pm2 = XCreatePixmap(dpy, pm2, icons[i].width+18, icons[i].height+15, 24);
    XFillRectangle(dpy,icons[i].pm2, gc, 0,0,icons[i].width+18, icons[i].height+15);
    XCopyArea(dpy, pm2, icons[i].pm2, gc, -9, 0, icons[i].width+18, icons[i].height, 0, 0);

    // list mode
    icons[i].pm3 = XCreatePixmap(dpy, icons[i].iconwin, icons[i].t_width, icons[i].t_height, 24);
    XFillRectangle(dpy,icons[i].pm3, gc, 0,0,icons[i].t_width, icons[i].t_height);
    //XCopyArea(dpy, pm1, icons[i].pm3, gc, -9, 0, icons[i].t_width, icons[i].t_height, 0, 0);

    icons[i].pm4 = XCreatePixmap(dpy, icons[i].iconwin, icons[i].t_width, icons[i].t_height, 24);
    XFillRectangle(dpy,icons[i].pm4, gc, 0,0,icons[i].t_width, icons[i].t_height);
    //XCopyArea(dpy, pm1, icons[i].pm4, gc, -9, 0, icons[i].t_width, icons[i].t_height, 0, 0);


    if (strcmp(icons[i].type,"directory")==0)
    {
      //XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[FILLPEN]);
      XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);
      XDrawImageString(dpy, icons[i].pm3, gc, 5, 12, icons[i].name, strlen(icons[i].name));
      XSetBackground(dpy,gc,dri.dri_Pens[FILLPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, icons[i].pm4, gc, 5, 12, icons[i].name, strlen(icons[i].name));

    }
    else if (strcmp(icons[i].type,"file")==0)
    {
      //XSetWindowBackground(dpy, icons[i].iconwin, dri.dri_Pens[SHADOWPEN]);
      XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, icons[i].pm3, gc, 5, 12, icons[i].name, strlen(icons[i].name));
      XSetBackground(dpy,gc,dri.dri_Pens[SHADOWPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, icons[i].pm4, gc, 5, 12, icons[i].name, strlen(icons[i].name));
    }


    icons[i].pmA = icons[i].pm1; /* set active pixmap */
    XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);

    //shorten long labels
    if (strlen(icons[i].name) > 10 )
    {
      char *str1=icons[i].name;
      char str2[i][13];
      strncpy (str2[i],str1,10);
      str2[i][10]='.';
      str2[i][11]='.';
      str2[i][12]='\0';
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
    if(icon_do != NULL){ FreeDiskObject(icon_do); }
  }
}

void spawn_new_wb(const char *cmd, char *title)
{
  const char *exec = "/usr/local/lib/amiwm/workbench";
  char *line=alloca(strlen(exec) + strlen(cmd) + strlen(title) +4);
  sprintf(line, "%s %s %s &", exec, cmd, title);
  printf("spawn_new_wb: my exec line=%s\n",line);
  system(line);
}

void deselectAll()  // clicked on the window. abort, clear all icon selection
{
  printf("run deselectAll\n");
  icon_temp.pmA = icon_temp.pm1;
  icon_temp.selected=False;
  icon_temp.dragging=False;
  for (int i=0;i<dircount;i++)
  {
    icons[i].pmA = icons[i].pm1;
    icons[i].selected = False;
    icons[i].dragging = False;

    if (strcmp(get_viewmode(),"icons")==0)
    {
      XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
      XClearWindow(dpy, icons[i].iconwin);
      XFlush(dpy);
    }
  }
}

void deselectOthers()  // clicked on the window. abort, clear all icon selection
{
  printf("run deselectOthers\n");
  for (int i=0;i<dircount;i++)
  {
    if ( ! (icons[i].iconwin == icon_temp.iconwin) )
    {
      icons[i].pmA = icons[i].pm1;
      icons[i].selected = False;
      icons[i].dragging = False;

      if (strcmp(get_viewmode(),"icons")==0)
      {
        XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
        XClearWindow(dpy, icons[i].iconwin);
        XFlush(dpy);
      }
    }
  }
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

  // find user's home
  char homedir[50];
  snprintf(homedir, sizeof(homedir) , "%s", getenv("HOME"));

  // set default window title
  if(argc<3) {  argv[2]= "home"; }

  // open default directory
  if(argv[1]==NULL) { argv[1]= homedir; }

  root = RootWindow(dpy, DefaultScreen(dpy));


  XGetWindowAttributes(dpy, root, &attr);
  init_dri(&dri, dpy, root, attr.colormap, False);

  mainwin=XCreateSimpleWindow(dpy, root, win_x, win_y, win_width, win_height, 1,
                              dri.dri_Pens[BACKGROUNDPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|KeyReleaseMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask|EnterWindowMask|LeaveWindowMask);
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

  read_entries(argv[1]);// todo: try to remove one of the two loop
  getlabels(argv[1]);   // todo: try to remove one of the two loop
  build_icons();
  list_entries_icons();
  XMapSubwindows(dpy, mainwin);
  XMapRaised(dpy, mainwin);
  XSync(dpy, False);
  event_loop();
}

void event_loop()
{
  for(;;)
  {
    XEvent event;
    XNextEvent(dpy, &event);
      switch(event.type)
      {
        case Expose:
          if(!event.xexpose.count)
          {
            if(event.xexpose.window == mainwin)
            {
//               if(strcmp(viewmode,"list")==0)
//               {
//                 list_entries();
//               }
//               if (strcmp(viewmode,"icons")==0)
//               {
//                 list_entries_icons();
//               }
            }
          }
        break;

        case LeaveNotify:
          //printf("leave event\n");
        break;

        case EnterNotify:
          //printf("enter event\n");
        break;

        case ButtonPress:
          //printf("buttonpress event\n");
          if (event.xcrossing.window==mainwin) // click on mainwin
          {
            deselectAll();
          }
          else
          {
            for (int i=0;i<dircount;i++)
            {
              if(event.xcrossing.window==icons[i].iconwin) // click on icon
              {
                if ((event.xbutton.time - last_icon_click) < dblClickTime) //double click
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
                    system(line);
                  }
                  else if (strcmp(icons[i].type,"directory")==0)
                  {
                    spawn_new_wb(icons[i].path,icons[i].name );
                  }
                }
                else  // single click
                {
                  last_icon_click=event.xbutton.time;



                  printf("simple click!\n");

                  if (strcmp(get_viewmode(), "icons")==0)
                  {
                    printf("toggle pm\n");

                    if (icons[i].pmA == icons[i].pm1) { icons[i].pmA = icons[i].pm2; }
                    else if (icons[i].pmA == icons[i].pm2) { icons[i].pmA = icons[i].pm1; }

                    icons[i].selected = TRUE;
                    icon_temp = icons[i]; // copy selected icon
                    deselectOthers();

                    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
                    XClearWindow(dpy, icons[i].iconwin);
                    XFlush(dpy);
                  }
                  else if (strcmp(get_viewmode(), "list")==0)
                  {
                    if (icons[i].pmA == icons[i].pm3) { icons[i].pmA = icons[i].pm4; }
                    else if (icons[i].pmA == icons[i].pm4) { icons[i].pmA = icons[i].pm3; }
                    icons[i].selected = TRUE;
                    icon_temp = icons[i]; //select icon (copy it to temp)
                    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
                    XClearWindow(dpy, icons[i].iconwin);
                    XFlush(dpy);
    //                XRaiseWindow(dpy, icons[i].iconwin);
    //                 xwa.x=0;
    //                 xwa.y=0;
    //                 XTranslateCoordinates(dpy, icons[i].iconwin,RootWindow(dpy, 0) ,xwa.x, xwa.y, &xwa.x, &xwa.y, &ww);
    //                 XReparentWindow(dpy, icons[i].iconwin,RootWindow(dpy, 0),xwa.x,xwa.y);

                  }
                }

              }
            }
          }
        break;

        case ConfigureNotify: // resize or move event
          //printf("configurenotify event\n");
          win_x=event.xconfigure.x;
          win_y=event.xconfigure.y;
          win_width=event.xconfigure.width;
          win_height=event.xconfigure.height;
        break;

        case MotionNotify:
          if(icon_temp.selected)
          {
            if( !(icon_temp.dragging))
            {
              printf("toggling dragging in motion notify\n");
              //XRaiseWindow(dpy, icons[i].iconwin);
              xwa.x=0;
              xwa.y=0;
              XTranslateCoordinates(dpy, icon_temp.iconwin,RootWindow(dpy, 0) ,xwa.x, xwa.y, &xwa.x, &xwa.y, &ww);
              XReparentWindow(dpy, icon_temp.iconwin,RootWindow(dpy, 0),xwa.x,xwa.y);
              icon_temp.dragging=True;
            }
            printf("motion notify\n");
            icon_temp.dragging=True;
            XMoveWindow(dpy,icon_temp.iconwin,event.xmotion.x_root-25, event.xmotion.y_root-25);
          }
        break;

        case ButtonRelease:
          if (icon_temp.selected && icon_temp.dragging)
          {
            xwa.x=0;
            xwa.y=0;
            XTranslateCoordinates(dpy, icon_temp.iconwin,RootWindow(dpy, 0) ,xwa.x, xwa.y, &xwa.x, &xwa.y, &ww);
            if (xwa.x > win_x && xwa.x < win_x+win_width &&
                xwa.y > win_y && xwa.y < win_y+win_height)
            {
              printf("icon within bounds of window, reparenting\n");
                 XReparentWindow(dpy, icon_temp.iconwin,mainwin, xwa.x-win_x,xwa.y-win_y);
            }
              XSetWindowBackgroundPixmap(dpy, icon_temp.iconwin, icon_temp.pmA);
              XClearWindow(dpy, icon_temp.iconwin);
              XFlush(dpy);
          }
        break;

        case KeyPress:
          printf("keypress event=%d\n",event.type);
          if(strcmp(get_viewmode(),"icons")==0)
          {
            printf("toggling to list\n");
            reset_view();
            list_entries();
          }
          else if (strcmp(get_viewmode(),"list")==0)
          {
            printf("toggling to icons\n");
            reset_view();
            list_entries_icons();
          }
        break;
      }
    }
  }

