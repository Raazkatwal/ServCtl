#include "window.h"

GtkWindow *servctl_window_new(GtkApplication *app) {
  GtkWidget *window;

  window = gtk_application_window_new(app);
  gtk_window_set_title(GTK_WINDOW(window), "ServCtl");
  gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);

  return GTK_WINDOW(window);
}
