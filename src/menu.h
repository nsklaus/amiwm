#ifndef MENU_H
#define MENU_H

struct Menu;
struct Item;

typedef struct _MenuBar {
  Window menubarparent;
  int menuleft;
  struct Menu *firstmenu;
  char *xprop;
  Client *client;
} MenuBar;

#define CHECKIT 1
#define CHECKED 2
#define DISABLED 4

extern void menu_initbar(MenuBar *mb);
extern void free_menus(MenuBar *mb);
extern struct Menu *add_menu(MenuBar *mb, const char *name, char flags);
extern struct Menu *sub_menu(struct Item *i, char flags);
extern struct Item *add_item(struct Menu *m, const char *name, char key, char flags);
extern void menu_layout(struct Menu *menu);

#endif
