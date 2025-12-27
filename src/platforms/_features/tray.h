
struct tray_menu;

struct tray {
  char *icon;
  struct tray_menu *menu;
};

struct tray_menu {
  char *text;
  int disabled;
  int checked;

  void (*cb)(struct tray_menu *);
  void *context;

  struct tray_menu *submenu;
};

static void tray_update(struct tray *tray);
