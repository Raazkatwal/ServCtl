#include "servctl-application.h"
#include "servctl-window.h"

struct _ServctlApplication {
  AdwApplication parent_instance;
};

G_DEFINE_TYPE(ServctlApplication, servctl_application, ADW_TYPE_APPLICATION)

static void on_about(GSimpleAction *action, GVariant *parameter, gpointer app) {
  GtkWindow *window = gtk_application_get_active_window(GTK_APPLICATION(app));

  static const char *developers[] = {"Raj Katwal", NULL};

  adw_show_about_dialog(GTK_WIDGET(window), "application-name", "Servctl",
                        "application-icon", "servctl", "developer-name", "Raj katwal",
                        "version", "0.1.0", "developers", developers,
                        "copyright", "© 2026 Raj Katwal", "website",
                        "https://github.com/Raazkatwal/servctl", "comments",
                        "A simple GNOME utility to manage services", NULL);
}

static void servctl_application_activate(GApplication *app) {
  ServctlWindow *window = servctl_window_new(SERVCTL_APPLICATION(app));

  gtk_window_present(GTK_WINDOW(window));
}

static void servctl_application_init(ServctlApplication *self) {
  static const GActionEntry app_actions[] = {
      {"about", on_about},
  };

  g_action_map_add_action_entries(G_ACTION_MAP(self), app_actions,
                                  G_N_ELEMENTS(app_actions), self);
}

static void servctl_application_class_init(ServctlApplicationClass *klass) {
  GApplicationClass *app_class = G_APPLICATION_CLASS(klass);
  app_class->activate = servctl_application_activate;
}

ServctlApplication *servctl_application_new(void) {
  return g_object_new(SERVCTL_TYPE_APPLICATION, "application-id",
                      "com.raaz.servctl", "flags", G_APPLICATION_DEFAULT_FLAGS,
                      NULL);
}
