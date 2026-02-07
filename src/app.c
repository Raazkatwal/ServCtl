#include "app.h"
#include "window.h"

#include <gtk/gtk.h>

static void on_activate(GtkApplication *app, gpointer user_data) {
	(void) user_data;
  GtkWindow *window;
  window = servctl_window_new(app);
  gtk_window_present(window);
}

int servctl_run(int argc, char **argv) {
  GtkApplication *app;
  int status;

  app = gtk_application_new("com.raaz.servctl", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
