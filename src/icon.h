#ifndef ICON_H
#define ICON_H

#include "client.h"
#include "libami.h"
//#include "module.h"

struct _Scrn;
typedef struct _Icon {
  struct _Icon *next, *nextselected;
  struct _Scrn *scr;
  Client *client;
  struct module *module;
  Window parent, window, labelwin, innerwin;
  Pixmap iconpm, secondpm, maskpm;
  char *label;
  int x, y, width, height;
  int labelwidth;
  int selected, mapped;
} Icon;

struct IconPixmaps
{
  Pixmap pm, pm2;
  struct ColorStore cs, cs2;
};

extern void redrawicon(Icon *, Window);
extern void rmicon(Icon *);
extern void createicon(Client *);
extern void createiconicon(Icon *i, XWMHints *);
extern void destroyiconicon(Icon *);
extern void cleanupicons();
extern void createdefaulticons();
extern void adjusticon(Icon *);
extern void selecticon(Icon *);
extern void deselecticon(Icon *);
extern void free_icon_pms(struct IconPixmaps *pms);

#endif
