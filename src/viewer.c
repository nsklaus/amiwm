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
#include <dirent.h>

#include "drawinfo.h"
#include "libami.h"


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
  Bool dragging;
} wbicon;

wbicon *icons;

int dircount;
Window root, mainwin;//, myicon;
int win_x=100, win_y=80, win_width=500, win_height=250;
GC gc;

void read_entries(char *path) {
  // differentiate between files and directories,
  // get max number of instances of wbicon
  DIR *dirp;
  struct dirent *dp;

  dirp = opendir(path);
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_REG)
    {
      // exclude common system entries and (semi)hidden names
      if (dp->d_name[0] != '.'){

        int len_cmd = strlen("/usr/bin/file ") + strlen(path) + strlen(dp->d_name) + strlen(" >/dev/null") +3;
        char *temp = malloc(len_cmd);
        strcpy(temp, "/usr/bin/file ");
        strcat(temp, "\"");
        strcat(temp,path);
        strcat(temp,dp->d_name);
        strcat(temp,"\"");
        char *my_cmd = temp;
        FILE *fp = popen(my_cmd, "r");
        char *ln = NULL;
        size_t len = 0;
        getline(&ln, &len, fp);

        char *buf = ln;
        char * ptr;
        int    ch = ':';
        ptr = strrchr( buf, ch );
        if (ptr !=NULL)
        {
          if(strstr(ptr, "Amiga") != NULL &&
            (strstr(ptr, "icon") !=NULL ))
          {
            dircount++;
          }
        }
        free(ln);
        pclose(fp);
      }
    }
  }
  // get max number of instances of wbicon to allocate
  printf("total file count=%d\n",dircount);
  icons = calloc(dircount, sizeof(wbicon));
  closedir(dirp);
  }

void getlabels(char *path)
{
  // same loop, but for getting filenames this time
  if (dircount > 0) // enter the loop only if we have files to process
  {
    DIR *dirp;
    struct dirent *dp;
    int count=0;
    dirp = opendir(path);
    while ((dp = readdir(dirp)) != NULL)
    {
      if (dp->d_type & DT_REG)
      {
        if (dp->d_name[0] != '.')
        {
          char *buf = dp->d_name;
          char * ptr;
          int    ch = '.';
          ptr = strrchr( buf, ch );
          if (ptr !=NULL)
          {
            if ( strcmp(ptr, ".info") == 0 )
            {
              //printf ("FILE: %s\n", dp->d_name);
              //wbicon_data(count, dp->d_name, path, "file" );
              icons[count].name =  malloc(strlen(dp->d_name)+1);
              strcpy(icons[count].name, dp->d_name);

              int pathsize = strlen(path) +2;
              char *mypath = malloc(pathsize);
              strcpy(mypath,path);
              icons[count].path = mypath;
              icons[count].type = "file";
              count++;
            }
          }
        }
      }
    }
    closedir(dirp);
  }
}

void list_entries(char *path)
{
  // icon palette
  unsigned long iconcolor[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };

  int newline_x = 0;
  int newline_y = 0;
  struct DiskObject *icon_do = NULL;
  char *icondir=path;

  for (int i=0;i<dircount;i++)
  {
    char *icon = icons[i].name;

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
    else if(newline_x*80+50 > win_width)
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
    int length = strlen(icons[i].name);
    char *temp = alloca(length);
    strcpy(temp,icons[i].name);
    temp[length-5] = '\0';

    if (strlen(temp) > 10 )
    {
      char *str1=temp;
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
      int my_offset = XmbTextEscapement(dri.dri_FontSet, temp, strlen(temp));
      int new_offset = (icons[i].width+18 - my_offset)/2;
      XmbDrawString(dpy, icons[i].pm1, dri.dri_FontSet, gc, new_offset, icons[i].height+10, temp, strlen(temp));
      XmbDrawString(dpy, icons[i].pm2, dri.dri_FontSet, gc, new_offset, icons[i].height+10, temp, strlen(temp));
    }
    XSelectInput(dpy, icons[i].iconwin, ExposureMask|KeyPressMask|ButtonPressMask|Button1MotionMask);
  }
  // XFreePixmap(dpy, pm1);
  // XFreePixmap(dpy, pm2);
  if(icon_do != NULL){ FreeDiskObject(icon_do); }
}

void deselectAll()
{
  for (int i=0;i<dircount;i++)
  {
    // clicked on the window, abort clear all icon selection
    icons[i].pmA = icons[i].pm1;
    icons[i].selected = False;
    icons[i].dragging = False;
    XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
    XClearWindow(dpy, icons[i].iconwin);
    XFlush(dpy);
  }
}

int main(int argc, char *argv[])
{
  // find user's home as default directory
  char homedir[50];
  snprintf(homedir, sizeof(homedir) , "%s", getenv("HOME"));
  if(argv[1]==NULL) { argv[1]= homedir; }

  // create window title from argv[1] (path)
  // take last dir and strip slashes, and add "view: " prefix
  int length = strlen(argv[1]);
  char *temp;// = argv[1];

  if(argv[1][length-1] == '/')
  {
    temp = alloca(length-1);
    strcpy(temp, argv[1]);
    temp[length-1] = '\0';
  } else {
    temp = alloca(length);
    strcpy(temp, argv[1]);
  }
  char * ptr;
  int    ch = '/';
  ptr = strrchr( temp, ch );
  printf("ptr1=%s\n",ptr);
  ptr[0] = ' ';
  printf("ptr2=%s\n",ptr);
  length = strlen("view: ") + strlen(ptr) + strlen(argv[1]);
  char *temp2 = alloca(length);
  strcpy(temp2, "view: ");
  strcat(temp2, ptr);
  argv[2] = temp2;

  //scr=c->scr;
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
                              dri.dri_Pens[BACKGROUNDPEN],
                              dri.dri_Pens[BACKGROUNDPEN]);

  XSelectInput(dpy, mainwin, ExposureMask|StructureNotifyMask|KeyPressMask|ButtonPressMask|Button1MotionMask);
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


  // make so provided path argument can work
  // with and without final slash
  length = strlen(argv[1]);
  char *mypath;
  if(argv[1][length-1] != '/') {
    mypath = alloca(length+1);
    strcpy(mypath, argv[1]);
    mypath[length]='/';
    mypath[length+1]='\0';
  } else if (argv[1][length-1] == '/'){
    mypath = alloca(length);
    strcpy(mypath, argv[1]);
  }
  read_entries(mypath);
  getlabels(mypath);  /* todo: try to remove one of the two loop  */
  list_entries(mypath);

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
          break;
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
            if (event.xcrossing.window==mainwin)
            {
              // clicked on the window, abort clear all icon selection
              deselectAll();
            }

            if(event.xcrossing.window==icons[i].iconwin)
            {
              // handle single click
              last_icon_click=event.xbutton.time;
              // clicked on icon, unselect others
              // TODO: handle modifier & multiselect later
              deselectAll();
              // toggle active icon
              if (icons[i].pmA == icons[i].pm1) { icons[i].pmA = icons[i].pm2; }
              else if (icons[i].pmA == icons[i].pm2) { icons[i].pmA = icons[i].pm1; }
              icons[i].dragging = TRUE;

              // force redraw
              XSetWindowBackgroundPixmap(dpy, icons[i].iconwin, icons[i].pmA);
              XClearWindow(dpy, icons[i].iconwin);
              XFlush(dpy);
            }

            // TODO: decoupling icon building from icon listing
            // to enable mouse wheel events like reqasl
//             if(event.xbutton.button==Button4)
//             {
//               printf("going up y=%d\n",event.xconfigure.height);
//               offset_y+=5;
//               list_entries();
//
//             }
//             if(event.xbutton.button==Button5)
//             {
//               printf("going down y=%d\n",event.xconfigure.height);
//               offset_y-=5;
//               list_entries();
//             }
          }
          break;
        case ConfigureNotify: // resize or move event
          win_x=event.xconfigure.x;
          win_y=event.xconfigure.y;
          win_width=event.xconfigure.width;
          win_height=event.xconfigure.height;
          break;
        case MotionNotify:
          for (int i=0;i<dircount;i++)
          {
            if(icons[i].dragging) {
              XRaiseWindow(dpy, icons[i].iconwin);
              XMoveWindow(dpy,icons[i].iconwin,event.xmotion.x_root-win_x-25,event.xmotion.y_root-win_y-25);
            }
          }
          break;
        case ButtonRelease:
          for (int i=0;i<dircount;i++)
          {
            if(event.xcrossing.window==icons[i].iconwin)
            {
              icons[i].dragging = FALSE;
            }
          }
          break;
        case KeyPress:
          break;
      }
    }
  }
}
