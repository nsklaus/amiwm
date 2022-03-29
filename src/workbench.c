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

#include <sys/stat.h> 
#include <errno.h> 
#include <locale.h>
#include <wchar.h> 

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
  char *tiedWith;
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
  Bool Icon;
  Bool associated;
  Bool selected;
  Bool dragging;
} fs_entity;

fs_entity *fse_arr; // array for normal files
fs_entity *fsi_arr; // array for info files
int alloc_ecount = 20; // array size
int alloc_icount = 20;

int entries_count; // total amount of entries per dir
int icon_count; // total amount of icons per dir

Window ww; //xreparent and xtranslatecoordinates
fs_entity icon_temp; // copy of selected icon
XWindowAttributes xwa;

// store browsing progression
char *current_dir="";
char *parent_dir="";

Window root, mainwin;//, myicon;
int win_x=100, win_y=80, win_width=300, win_height=150;
GC gc;

void build_icons();
void event_loop();

void read_entries(char *path) {
  // differentiate between files and directories,
  // get max number of instances of wbicon
  DIR *dirp;
  struct dirent *dp;
  int file_count=0;
  
  if((dirp = opendir(path)) == NULL) 
  {
    fprintf(stderr,"cannot open directory: %s\n", path);
    return;
  }

  while ((dp = readdir(dirp)) != NULL)
  {
    if (entries_count==alloc_ecount)
    {
      alloc_ecount+=(alloc_ecount/2);
      fse_arr = realloc(fse_arr,alloc_ecount * sizeof(fs_entity));
      printf("new alloc_ecount=%d\n",alloc_ecount);
    }
    
//     if (dp->d_type & DT_DIR || (dp->d_type & DT_REG))
//     {

      
    // exclude common system entries and (semi)hidden names
    if (dp->d_name[0] != '.')
    {
      if(strcmp(".",dp->d_name) == 0 || strcmp("..",dp->d_name) == 0)
      {
        continue; //for future, when handling show hidden y/n
      }
      
      
      // ================
      // case directory
      // ================
      if (dp->d_type & DT_DIR)
      {
        int length=strlen(dp->d_name)+1;
        fse_arr[entries_count].name =  malloc(length);
        strcpy(fse_arr[entries_count].name, dp->d_name);
        int pathsize = strlen(path) + strlen(fse_arr[entries_count].name) +2;
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        strcat(tempo,fse_arr[entries_count].name);
        strcat(tempo,"/");
        fse_arr[entries_count].path = tempo;
        fse_arr[entries_count].type = "directory";
        entries_count++;
        file_count++;
      }
      if (dp->d_type & DT_REG)
      {
        // ==============
        // case icon file
        // ==============
        char *buf = dp->d_name;
        char * ptr;
        int    ch = '.';
        ptr = strrchr( buf, ch );
        if (ptr !=NULL)
        {
          if ( strcmp(ptr, ".info") == 0 && (dp->d_type & DT_REG) )
          {
            fse_arr[entries_count].Icon = True;
            fse_arr[entries_count].tiedWith = ""; // initialize
          }
        }
        // ================
        // case normal file
        // ================
        fse_arr[entries_count].name =  malloc(strlen(dp->d_name)+1);
        strcpy(fse_arr[entries_count].name, dp->d_name);
        int pathsize2 = strlen(path)+2;
        
        char *tempo2 = malloc(pathsize2);
        strcpy(tempo2,path);
        fse_arr[entries_count].path = tempo2;
        fse_arr[entries_count].type = "file";
        
        entries_count++;
        file_count++;
      }
    }
  }
  closedir(dirp);
  printf("file_count=%d\n",file_count);
}

void getlabels(char *path)
{
  /*
  // same loop, but for getting filenames this time
  DIR *dirp;
  struct dirent *dp;
  int count=0;
  int i_count=0;
  dirp = opendir(path);
  while ((dp = readdir(dirp)) != NULL)
  {
    if (dp->d_type & DT_DIR)
    {
      if (dp->d_name[0] != '.')
      {
        printf("name before malloc() name=%s\n",dp->d_name);
        int length=strlen(dp->d_name)+1;
        fse_arr[count].name =  malloc(length);
        
        strcpy(fse_arr[count].name, dp->d_name);
        printf("last char for fse_arr[count].name=%c\n",fse_arr[count].name[length-1]);
        int pathsize = strlen(path) + strlen(fse_arr[count].name) +2;

        printf("pathsize=%d count=%d entries_count=%d name=%s\n",pathsize, count,entries_count,fse_arr[count].name );
        char *tempo = malloc(pathsize);
        strcpy(tempo,path);
        strcat(tempo,fse_arr[count].name);
        strcat(tempo,"/");
        fse_arr[count].path = tempo;
        fse_arr[count].type = "directory";
        fse_arr[count].x = 20;
        fse_arr[count].y = 20;
        fse_arr[count].width = 20;
        fse_arr[count].height = 20;
        count++;
      }
    }

    else if (dp->d_type & DT_REG)
    {
      if (dp->d_name[0] != '.')
      {
        //printf ("FILE: %s\n", dp->d_name);
        //wbicon_data(count, dp->d_name, path, "file" );
        char *buf = dp->d_name;
        //printf("FILE: buf=%s\n",buf);
        char * ptr;
        int    ch = '.';
        ptr = strrchr( buf, ch );
        if (ptr !=NULL)
        {
          if ( strcmp(ptr, ".info") == 0 ) // this is .info file
          {
            fsi_arr[i_count].name =  malloc(strlen(dp->d_name)+1);
            strcpy(fsi_arr[i_count].name, dp->d_name);
            int pathsize1 = strlen(path)+2;

            char *tempo1 = malloc(pathsize1);
            strcpy(tempo1,path);
            fsi_arr[i_count].path = tempo1;
            fsi_arr[i_count].type = "file";
            i_count++;
          }
          else if (strcmp(ptr, ".info") != 0) // not .info file
          { 

            fse_arr[count].name =  malloc(strlen(dp->d_name)+1);
            strcpy(fse_arr[count].name, dp->d_name);
            int pathsize2 = strlen(path)+2;

            char *tempo2 = malloc(pathsize2);
            strcpy(tempo2,path);
            fse_arr[count].path = tempo2;
            fse_arr[count].type = "file";
            count++;
          }
        }
        else  // case with files without "." in filenames
        {

          fse_arr[count].name =  malloc(strlen(dp->d_name)+1);
          strcpy(fse_arr[count].name, dp->d_name);
          int pathsize2 = strlen(path)+2;

          char *tempo2 = malloc(pathsize2);
          strcpy(tempo2,path);
          fse_arr[count].path = tempo2;
          fse_arr[count].type = "file";
          count++;
        }
      }
    }
  }
  closedir(dirp);
*/
}

char * get_viewmode()
{
  return viewmode;
}

void reset_view()
{
  for (int i=0;i<entries_count-1;i++)
  {
//     XFreePixmap(dpy, fse_arr[i].pm1);
//     XFreePixmap(dpy, fse_arr[i].pm2);
//     XFreePixmap(dpy, fse_arr[i].pmA);
    XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, None);
    XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[BACKGROUNDPEN]);
    XClearWindow(dpy, fse_arr[i].iconwin);
  }
  XFlush(dpy);
}

void list_entries()
{
  viewmode="list";
  printf("VIEWMODE now =%s\n",get_viewmode());

  for (int i=0;i<entries_count;i++)
  {
    fse_arr[i].width  = fse_arr[i].t_width;
    fse_arr[i].height = fse_arr[i].t_height;
    fse_arr[i].x = 10;
    fse_arr[i].y = 10 + i*16;
    fse_arr[i].pmA = fse_arr[i].pm3;
    XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pm3);
    XResizeWindow(dpy,fse_arr[i].iconwin,fse_arr[i].width,fse_arr[i].height);
    //XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[FILLPEN]);


//     if (strcmp(fse_arr[i].type,"directory")==0)
//     {
//       XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[FILLPEN]);
//       XSetBackground(dpy,gc,dri.dri_Pens[FILLPEN]);
//       XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
//       XDrawImageString(dpy, fse_arr[i].iconwin, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
//     }
//     else if (strcmp(fse_arr[i].type,"file")==0)
//     {
//       XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[SHADOWPEN]);
//       XSetBackground(dpy,gc,dri.dri_Pens[SHADOWPEN]);
//       XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
//       XDrawImageString(dpy, fse_arr[i].iconwin, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
//     }

    XMoveWindow(dpy,fse_arr[i].iconwin,fse_arr[i].x,fse_arr[i].y);
    XClearWindow(dpy, fse_arr[i].iconwin);
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

   for (int i=0;i<entries_count;i++)
   {
     printf("values of iconwin=%lu\n",fse_arr[i].iconwin);
     if(fse_arr[i].iconwin != 0)
     {

      if(newline_x*80 < win_width)
      {
        fse_arr[i].x=10 + (newline_x*80);
        fse_arr[i].y=10 + (newline_y*50);
        newline_x++;
      }
      else if(newline_x*80*2 > win_width)
      {
        newline_x = 0;
        newline_y++;
        fse_arr[i].x=10 + (newline_x*80);
        fse_arr[i].y=10 + (newline_y*50);
        newline_x++;
      }
      fse_arr[i].width  = fse_arr[i].p_width;
      fse_arr[i].height = fse_arr[i].p_height;
      XMoveWindow(dpy,fse_arr[i].iconwin,fse_arr[i].x,fse_arr[i].y);
      XResizeWindow(dpy,fse_arr[i].iconwin,fse_arr[i].width+18,fse_arr[i].height+15);
      fse_arr[i].pmA = fse_arr[i].pm1;
      XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
      XClearWindow(dpy, fse_arr[i].iconwin);
      XFlush(dpy);
    }
  }
}

void geticon(char *path, char *file)
{
  printf("geticon: path=%s file=%s\n",path,file);
}

void make_icon( char *icondir, char *icon, int i)
{
  printf("make_icons, icondir=%s icon=%s\n", icondir,icon);
  XSetWindowAttributes xswa;
  xswa.override_redirect = True;
  struct DiskObject *icon_do = NULL;  
  unsigned long iconcolor[8] = { 11184810, 0, 16777215, 6719675, 10066329, 12303291, 12298905, 16759722 };
  
  fse_arr[i].iconwin = XCreateWindow( dpy,mainwin, fse_arr[i].x, fse_arr[i].y, fse_arr[i].width+18, fse_arr[i].height+15, 1, 24, InputOutput, CopyFromParent, CWBackPixel|CWOverrideRedirect, &xswa);
  
  XMapWindow(dpy, fse_arr[i].iconwin);
  
  XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
  XSelectInput(dpy, fse_arr[i].iconwin, ExposureMask|CWOverrideRedirect|KeyPressMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask);
  
  int label_width = XmbTextEscapement(dri.dri_FontSet, fse_arr[i].name, strlen(fse_arr[i].name));
  fse_arr[i].t_width  = label_width+10;
  fse_arr[i].t_height = 15;
  
  
  
  int rl=strlen(icon)+strlen(icondir)+2;
  char *fn=alloca(rl);
  sprintf(fn, "%s/%s", icondir, icon);
  fn[strlen(fn)-5]=0; // strip suffix .info
  printf("getDiskObject full path: FN=%s\n",fn);
  icon_do = GetDiskObject(fn);
  

  
  struct Image *im1 = icon_do->do_Gadget.GadgetRender;
  struct Image *im2 = icon_do->do_Gadget.SelectRender;
  
  fse_arr[i].p_width  = icon_do->do_Gadget.Width;
  fse_arr[i].p_height = icon_do->do_Gadget.Height;
  fse_arr[i].width  = fse_arr[i].p_width;
  fse_arr[i].height = fse_arr[i].p_height;
  
  pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                        im1, fse_arr[i].width, fse_arr[i].height, &colorstore1);
  pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                        im2, fse_arr[i].width, fse_arr[i].height, &colorstore2);
  
  fse_arr[i].pm1 = XCreatePixmap(dpy, pm1, fse_arr[i].width+18, fse_arr[i].height+15, 24);
  XFillRectangle(dpy,fse_arr[i].pm1, gc, 0,0,fse_arr[i].width+18, fse_arr[i].height+15);
  XCopyArea(dpy, pm1, fse_arr[i].pm1, gc, -9, 0, fse_arr[i].width+18, fse_arr[i].height, 0, 0);
  
  fse_arr[i].pm2 = XCreatePixmap(dpy, pm2, fse_arr[i].width+18, fse_arr[i].height+15, 24);
  XFillRectangle(dpy,fse_arr[i].pm2, gc, 0,0,fse_arr[i].width+18, fse_arr[i].height+15);
  XCopyArea(dpy, pm2, fse_arr[i].pm2, gc, -9, 0, fse_arr[i].width+18, fse_arr[i].height, 0, 0);
  
  // list mode
  fse_arr[i].pm3 = XCreatePixmap(dpy, fse_arr[i].iconwin, fse_arr[i].t_width, fse_arr[i].t_height, 24);
  XFillRectangle(dpy,fse_arr[i].pm3, gc, 0,0,fse_arr[i].t_width, fse_arr[i].t_height);
  //XCopyArea(dpy, pm1, fse_arr[i].pm3, gc, -9, 0, fse_arr[i].t_width, fse_arr[i].t_height, 0, 0);
  
  fse_arr[i].pm4 = XCreatePixmap(dpy, fse_arr[i].iconwin, fse_arr[i].t_width, fse_arr[i].t_height, 24);
  XFillRectangle(dpy,fse_arr[i].pm4, gc, 0,0,fse_arr[i].t_width, fse_arr[i].t_height);
  //XCopyArea(dpy, pm1, fse_arr[i].pm4, gc, -9, 0, fse_arr[i].t_width, fse_arr[i].t_height, 0, 0);
  
  if (strcmp(fse_arr[i].type,"directory")==0)
  {
    //XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[FILLPEN]);
    XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
    //XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);
    //XSetForeground(dpy, gc, 0x372db0);
    XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
    XDrawImageString(dpy, fse_arr[i].pm3, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
    XSetBackground(dpy,gc,dri.dri_Pens[FILLPEN]);
    XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
    XDrawImageString(dpy, fse_arr[i].pm4, gc, 5, 12,fse_arr[i].name, strlen(fse_arr[i].name));
    
  }
  else if (strcmp(fse_arr[i].type,"file")==0)
  {
    //XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[SHADOWPEN]);
    XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
    XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
    XDrawImageString(dpy, fse_arr[i].pm3, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
    XSetBackground(dpy,gc,dri.dri_Pens[SHADOWPEN]);
    XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
    XDrawImageString(dpy, fse_arr[i].pm4, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
  }
  
  
  fse_arr[i].pmA =fse_arr[i].pm1; /* set active pixmap */
  XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
  
  //shorten long labels
//   if (strlen(entity.name) > 10 )
//   {
//     char *str1=fse_arr[i].name;
//     char str2[i][13];
//     strncpy (str2[i],str1,10);
//     str2[i][10]='.';
//     str2[i][11]='.';
//     str2[i][12]='\0';
//     XmbDrawString(dpy, fse_arr[i].pm1, dri.dri_FontSet, gc, 0, fse_arr[i].height+10, str2[i], strlen(str2[i]));
//     XmbDrawString(dpy, fse_arr[i].pm2, dri.dri_FontSet, gc, 0, fse_arr[i].height+10, str2[i], strlen(str2[i]));
//   }
//   else
//   {
    // get string length in pixels and calc offset
  int my_offset = XmbTextEscapement(dri.dri_FontSet, fse_arr[i].name, strlen(fse_arr[i].name));
  int new_offset = (fse_arr[i].width+18 - my_offset)/2;
  XmbDrawString(dpy, fse_arr[i].pm1, dri.dri_FontSet, gc, new_offset, fse_arr[i].height+10, fse_arr[i].name, strlen(fse_arr[i].name));
  XmbDrawString(dpy, fse_arr[i].pm2, dri.dri_FontSet, gc, new_offset, fse_arr[i].height+10,fse_arr[i].name, strlen(fse_arr[i].name));
//  }
  if(icon_do != NULL){ FreeDiskObject(icon_do); }
}

void build_icons()
{
  printf("passing through build icons\n");
  // icon palette
  char *icondir="";
  char *icon="";
 
  
  for (int i=0;i<entries_count;i++)
  {
 
    if (fse_arr[i].Icon == True)
    {
      char *buf = strdup(fse_arr[i].name);
      //printf("\noriginal icon name, buf=%s\n",buf);
      char * ptr;
      int    ch = '.';
      ptr = strrchr( buf, ch );
      int pos = ptr-buf;
      if (ptr !=NULL)
      {
        if ( strcmp(ptr, ".info") == 0 )
        {
          buf[pos]='\0'; // remove suffix .info
        }
      }
      // compare ou file's name with the rest
      for (int j=0;j<entries_count;j++)
      {
        // exclude icons from comparison
        if(fse_arr[j].Icon == False) 
        {
          // both names match, store it, mark association
          if (strcmp(fse_arr[j].name,buf)==0  )
          {
            fse_arr[j].associated = True; // tell normal file it will have an icon
            fse_arr[i].associated = True; // tell icon it have a normal file to be paired with
            fse_arr[i].tiedWith = fse_arr[j].name; 
            //printf("to be linked together: file=%s | icon=%s\n",fse_arr[j].name, fse_arr[i].name );
          }
        }
      }
    }
  }
  
  for (int i=0;i<entries_count;i++)
  {
    // normal entity, not an icon, but has an icon associated to it 
    if (fse_arr[i].associated && fse_arr[i].Icon == False)
    {
      for (int j=0;j<entries_count;j++)
      {
        // find corresponding icon with name/tiedWith match
        if (fse_arr[j].Icon && strcmp(fse_arr[i].name,fse_arr[j].tiedWith)==0 )
        {
          if (strcmp(fse_arr[i].type,"file")==0)
          {
            char *temp = malloc(strlen(fse_arr[i].path));
            strcpy(temp,fse_arr[i].path);
            temp[(strlen(temp)-1)]='\0';
            icondir=temp;
            icon=fse_arr[j].name;
            printf("case 1: icondir=%s icon=%s\n",icondir,icon);
            make_icon(icondir, icon, i); 
            
          }
          // case entity is a directory
          else if (strcmp(fse_arr[i].type,"directory")==0)
          {
            // strip last part of path
            char *temp = malloc(strlen(fse_arr[i].path));
            strcpy(temp,fse_arr[i].path);
            temp[(strlen(temp)-1)]='\0';
            char * ptr;
            int    ch = '/';
            ptr = strrchr( temp, ch ); // find where next "last" slash is now
            int pos = (ptr-temp);
            
            char *newbuff = malloc(pos);
            memcpy(newbuff,temp,pos);
            newbuff[pos] = '\0';
            icondir=newbuff;
            icon=fse_arr[j].name;
            printf("case 2: icondir=%s icon=%s\n",icondir,icon);
            make_icon(icondir, icon, i); 
          }
        }
      }
    }
    // normal file, not an icon, and has no icon associated: gets default icon
    if (fse_arr[i].associated == False && fse_arr[i].Icon == False)
    {
      icondir="/usr/local/lib/amiwm/icons";
      if(strcmp(fse_arr[i].type, "directory") == 0) { icon="def_drawer.info"; }
      else if(strcmp(fse_arr[i].type, "file") == 0) { icon="def_tool.info"; }
      printf("case 3: icondir=%s icon=%s\n",icondir,icon);
      make_icon(icondir, icon, i); 
    }
    
    // icon file, and has no file associated: show icon
    if (fse_arr[i].associated == False && fse_arr[i].Icon == True)
    {
      if (strcmp(fse_arr[i].type,"file")==0)
      {
        char *temp = malloc(strlen(fse_arr[i].path));
        strcpy(temp,fse_arr[i].path);
        temp[(strlen(temp)-1)]='\0';
        icondir=temp;
        icon=fse_arr[i].name;
        printf("case 4: icondir=%s icon=%s\n",icondir,icon);
        make_icon(icondir, icon, i); 
      }
      // case entity is a directory
//       else if (strcmp(fse_arr[i].type,"directory")==0)
//       {
//         //strip last part of path
//         char *temp = malloc(strlen(fse_arr[i].path));
//         strcpy(temp,fse_arr[i].path);
//         temp[(strlen(temp)-1)]='\0';
//         char * ptr;
//         int    ch = '/';
//         ptr = strrchr( temp, ch ); // find where next "last" slash is now
//         int pos = (ptr-temp);
//         
//         char *newbuff = malloc(pos);
//         memcpy(newbuff,temp,pos);
//         newbuff[pos] = '\0';
//         icondir=newbuff;
//         icon=fse_arr[i].name;
//         printf("case 5: icondir=%s icon=%s\n",icondir,icon);
//         make_icon(icondir, icon,fse_arr[i]); 
//       }
    }
  }
      

    
/*
    // ===========================================
    // icon processing done start creating windows 
    // ===========================================
    fse_arr[i].iconwin = XCreateWindow( dpy,mainwin, fse_arr[i].x, fse_arr[i].y, fse_arr[i].width+18, fse_arr[i].height+15, 1, 24, InputOutput, CopyFromParent, CWBackPixel|CWOverrideRedirect, &xswa);
    
    XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
    XSelectInput(dpy, fse_arr[i].iconwin, ExposureMask|CWOverrideRedirect|KeyPressMask|ButtonPressMask|ButtonReleaseMask|Button1MotionMask);
    
    int label_width = XmbTextEscapement(dri.dri_FontSet, fse_arr[i].name, strlen(fse_arr[i].name));
    fse_arr[i].t_width  = label_width+10;
    fse_arr[i].t_height = 15;
    
    struct Image *im1 = icon_do->do_Gadget.GadgetRender;
    struct Image *im2 = icon_do->do_Gadget.SelectRender;

    fse_arr[i].p_width  = icon_do->do_Gadget.Width;
    fse_arr[i].p_height = icon_do->do_Gadget.Height;
    fse_arr[i].width  = fse_arr[i].p_width;
    fse_arr[i].height = fse_arr[i].p_height;

    pm1 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                          im1, fse_arr[i].width, fse_arr[i].height, &colorstore1);
    pm2 = image_to_pixmap(dpy, mainwin, gc, dri.dri_Pens[BACKGROUNDPEN], iconcolor, 7,
                          im2, fse_arr[i].width, fse_arr[i].height, &colorstore2);

    // icon mode
    fse_arr[i].pm1 = XCreatePixmap(dpy, pm1, fse_arr[i].width+18, fse_arr[i].height+15, 24);
    XFillRectangle(dpy,fse_arr[i].pm1, gc, 0,0,fse_arr[i].width+18, fse_arr[i].height+15);
    XCopyArea(dpy, pm1, fse_arr[i].pm1, gc, -9, 0, fse_arr[i].width+18, fse_arr[i].height, 0, 0);

    fse_arr[i].pm2 = XCreatePixmap(dpy, pm2, fse_arr[i].width+18, fse_arr[i].height+15, 24);
    XFillRectangle(dpy,fse_arr[i].pm2, gc, 0,0,fse_arr[i].width+18, fse_arr[i].height+15);
    XCopyArea(dpy, pm2, fse_arr[i].pm2, gc, -9, 0, fse_arr[i].width+18, fse_arr[i].height, 0, 0);

    // list mode
    fse_arr[i].pm3 = XCreatePixmap(dpy, fse_arr[i].iconwin, fse_arr[i].t_width, fse_arr[i].t_height, 24);
    XFillRectangle(dpy,fse_arr[i].pm3, gc, 0,0,fse_arr[i].t_width, fse_arr[i].t_height);
    //XCopyArea(dpy, pm1, fse_arr[i].pm3, gc, -9, 0, fse_arr[i].t_width, fse_arr[i].t_height, 0, 0);

    fse_arr[i].pm4 = XCreatePixmap(dpy, fse_arr[i].iconwin, fse_arr[i].t_width, fse_arr[i].t_height, 24);
    XFillRectangle(dpy,fse_arr[i].pm4, gc, 0,0,fse_arr[i].t_width, fse_arr[i].t_height);
    //XCopyArea(dpy, pm1, fse_arr[i].pm4, gc, -9, 0, fse_arr[i].t_width, fse_arr[i].t_height, 0, 0);


    if (strcmp(fse_arr[i].type,"directory")==0)
    {
      //XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[FILLPEN]);
      XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
      //XSetForeground(dpy, gc, dri.dri_Pens[FILLPEN]);
      //XSetForeground(dpy, gc, 0x372db0);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, fse_arr[i].pm3, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
      XSetBackground(dpy,gc,dri.dri_Pens[FILLPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, fse_arr[i].pm4, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));

    }
    else if (strcmp(fse_arr[i].type,"file")==0)
    {
      //XSetWindowBackground(dpy, fse_arr[i].iconwin, dri.dri_Pens[SHADOWPEN]);
      XSetBackground(dpy,gc,dri.dri_Pens[BACKGROUNDPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);
      XDrawImageString(dpy, fse_arr[i].pm3, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
      XSetBackground(dpy,gc,dri.dri_Pens[SHADOWPEN]);
      XSetForeground(dpy, gc, dri.dri_Pens[SHINEPEN]);
      XDrawImageString(dpy, fse_arr[i].pm4, gc, 5, 12, fse_arr[i].name, strlen(fse_arr[i].name));
    }


    fse_arr[i].pmA = fse_arr[i].pm1; // set active pixmap 
    XSetForeground(dpy, gc, dri.dri_Pens[TEXTPEN]);

    //shorten long labels
    if (strlen(fse_arr[i].name) > 10 )
    {
      char *str1=fse_arr[i].name;
      char str2[i][13];
      strncpy (str2[i],str1,10);
      str2[i][10]='.';
      str2[i][11]='.';
      str2[i][12]='\0';
      XmbDrawString(dpy, fse_arr[i].pm1, dri.dri_FontSet, gc, 0, fse_arr[i].height+10, str2[i], strlen(str2[i]));
      XmbDrawString(dpy, fse_arr[i].pm2, dri.dri_FontSet, gc, 0, fse_arr[i].height+10, str2[i], strlen(str2[i]));
    }
    else
    {
      // get string length in pixels and calc offset
      int my_offset = XmbTextEscapement(dri.dri_FontSet, fse_arr[i].name, strlen(fse_arr[i].name));
      int new_offset = (fse_arr[i].width+18 - my_offset)/2;
      XmbDrawString(dpy, fse_arr[i].pm1, dri.dri_FontSet, gc, new_offset, fse_arr[i].height+10, fse_arr[i].name, strlen(fse_arr[i].name));
      XmbDrawString(dpy, fse_arr[i].pm2, dri.dri_FontSet, gc, new_offset, fse_arr[i].height+10, fse_arr[i].name, strlen(fse_arr[i].name));
    }
    if(icon_do != NULL){ FreeDiskObject(icon_do); }
  */
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
  for (int i=0;i<entries_count;i++)
  {
    if(fse_arr[i].iconwin != 0)
    {
      fse_arr[i].pmA = fse_arr[i].pm1;
      fse_arr[i].selected = False;
      fse_arr[i].dragging = False;

      if (strcmp(get_viewmode(),"icons")==0)
      {
        XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
        XClearWindow(dpy, fse_arr[i].iconwin);
        XFlush(dpy);
      }
    }
  }
}

void deselectOthers()  // clicked on the window. abort, clear all icon selection
{
  printf("run deselectOthers\n");
  for (int i=0;i<entries_count;i++)
  {
    if(fse_arr[i].iconwin != 0)
    {
      if ( ! (fse_arr[i].iconwin == icon_temp.iconwin) )
      {
        fse_arr[i].pmA = fse_arr[i].pm1;
        fse_arr[i].selected = False;
        fse_arr[i].dragging = False;

        if (strcmp(get_viewmode(),"icons")==0)
        {
          XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
          XClearWindow(dpy, fse_arr[i].iconwin);
          XFlush(dpy);
        }
      }
    }
  }
}

int main(int argc, char *argv[])
{
  // initial allocation of arrays
  fse_arr = calloc(alloc_ecount, sizeof(fs_entity));
  fsi_arr = calloc(alloc_icount, sizeof(fs_entity));
  
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
  // add final slash,  getenv("HOME") provides
  int pos = strlen(homedir);

  char *newbuff = malloc(pos);

  memcpy(newbuff,homedir,pos);  // sample: "/home/klaus/Downloads"
  newbuff[pos] = '/';
  newbuff[pos+1] = '\0';
  strcpy(homedir,newbuff);
  free(newbuff);

  printf("cleaned homedir path =%s\n",homedir);

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

  XClassHint *myhint = XAllocClassHint();
  myhint->res_class="workbench";
  myhint->res_name="Workbench";
  XSetClassHint(dpy,mainwin,myhint);
  
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
            for (int i=0;i<entries_count;i++)
            {
              if(fse_arr[i].iconwin != 0)
              {
                if(event.xcrossing.window==fse_arr[i].iconwin) // click on icon
                {
                  if ((event.xbutton.time - last_icon_click) < dblClickTime) //double click
                  {
                    printf("* double click! *\n");
                    fse_arr[i].pmA = fse_arr[i].pm2;
                    if (strcmp(fse_arr[i].type,"file")==0)
                    {
                      const char *cmd = "xdg-open";
                      const char *path = fse_arr[i].path;
                      const char *exec = fse_arr[i].name;
                      char *line=alloca(strlen(cmd) + strlen(fse_arr[i].path) + strlen(exec) +2);
                      sprintf(line, "%s %s%s &", cmd, path, exec);
                      system(line);
                    }
                    else if (strcmp(fse_arr[i].type,"directory")==0)
                    {
                      spawn_new_wb(fse_arr[i].path,fse_arr[i].name );
                    }
                  }
                  else  // single click
                  {
                    last_icon_click=event.xbutton.time;



                    printf("simple click!\n");

                    if (strcmp(get_viewmode(), "icons")==0)
                    {
                      printf("toggle pixmap\n");

                      if (fse_arr[i].pmA == fse_arr[i].pm1) { fse_arr[i].pmA = fse_arr[i].pm2; }
                      else if (fse_arr[i].pmA == fse_arr[i].pm2) { fse_arr[i].pmA = fse_arr[i].pm1; }

                      fse_arr[i].selected = TRUE;
                      icon_temp = fse_arr[i]; // copy selected icon
                      deselectOthers();

                      XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
                      XClearWindow(dpy, fse_arr[i].iconwin);
                      XFlush(dpy);
                    }
                    else if (strcmp(get_viewmode(), "list")==0)
                    {
                      if (fse_arr[i].pmA == fse_arr[i].pm3) { fse_arr[i].pmA = fse_arr[i].pm4; }
                      else if (fse_arr[i].pmA == fse_arr[i].pm4) { fse_arr[i].pmA = fse_arr[i].pm3; }
                      fse_arr[i].selected = TRUE;
                      icon_temp = fse_arr[i]; //select icon (copy it to temp)
                      XSetWindowBackgroundPixmap(dpy, fse_arr[i].iconwin, fse_arr[i].pmA);
                      XClearWindow(dpy, fse_arr[i].iconwin);
                      XFlush(dpy);
      //                XRaiseWindow(dpy, fse_arr[i].iconwin);
      //                 xwa.x=0;
      //                 xwa.y=0;
      //                 XTranslateCoordinates(dpy, fse_arr[i].iconwin,RootWindow(dpy, 0) ,xwa.x, xwa.y, &xwa.x, &xwa.y, &ww);
      //                 XReparentWindow(dpy, fse_arr[i].iconwin,RootWindow(dpy, 0),xwa.x,xwa.y);

                    }
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
              //XRaiseWindow(dpy, fse_arr[i].iconwin);
              xwa.x=0;
              xwa.y=0;
              XTranslateCoordinates(dpy, icon_temp.iconwin,RootWindow(dpy, 0) ,xwa.x, xwa.y, &xwa.x, &xwa.y, &ww);
              XReparentWindow(dpy, icon_temp.iconwin,RootWindow(dpy, 0),xwa.x,xwa.y);
              icon_temp.dragging=True;
            }
            //printf("motion notify\n");
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
          list_entries_icons();
          
//           if(strcmp(get_viewmode(),"icons")==0)
//           {
//             printf("toggling to list\n");
//             reset_view();
//             list_entries();
//           }
//           else if (strcmp(get_viewmode(),"list")==0)
//           {
//             printf("toggling to icons\n");
//             reset_view();
//             list_entries_icons();
//           }
        break;
      }
    }
  }

