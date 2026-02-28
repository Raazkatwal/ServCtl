#include "servctl-window.h"
#include <adwaita.h>

static void on_activate(AdwApplication *app) {
  ServctlWindow *window = servctl_window_new(app);

  gtk_window_present(GTK_WINDOW(window));
}

int main(int argc, char *argv[]) {
  g_autoptr(AdwApplication) app =
      adw_application_new("com.raaz.servctl", G_APPLICATION_DEFAULT_FLAGS);

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  return g_application_run(G_APPLICATION(app), argc, argv);
}
