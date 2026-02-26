#include "app.h"
#include "window.h"

#include <adwaita.h>
#include <gtk/gtk.h>

static void on_about(GSimpleAction *action, GVariant *parameter,
                     gpointer user_data) {
  (void)action;
  (void)parameter;

  GtkWindow *window =
      gtk_application_get_active_window(GTK_APPLICATION(user_data));

  adw_show_about_dialog(GTK_WIDGET(window), "application-name", "ServCtl",
                        "developer-name", "Raaz Katwal", "version", "0.1.0",
                        NULL);
}

static void on_preferences(GSimpleAction *action, GVariant *parameter,
                           gpointer user_data) {
  (void)action;
  (void)parameter;
  (void)user_data;

  g_print("Preferences clicked\n");
}

static void on_shortcuts(GSimpleAction *action, GVariant *parameter,
                         gpointer user_data) {
  (void)action;
  (void)parameter;
  (void)user_data;

  g_print("Shortcuts clicked\n");
}

static const GActionEntry app_actions[] = {
    {"about", on_about, NULL, NULL, NULL},
    {"preferences", on_preferences, NULL, NULL, NULL},
    {"shortcuts", on_shortcuts, NULL, NULL, NULL}};

static void on_activate(AdwApplication *app, gpointer user_data) {
  (void)user_data;

  GtkWindow *window = servctl_window_new(app);

  /* GtkBuilder *builder = gtk_builder_new_from_file("resources/ui/main.ui");
  GMenuModel *app_menu =
      G_MENU_MODEL(gtk_builder_get_object(builder, "app-menu"));

  adw_application_set_app_menu(GTK_APPLICATION(app), app_menu);

  g_object_unref(builder); */

  gtk_window_present(window);
}

int servctl_run(int argc, char **argv) {
  int status;

  AdwApplication *app =
      adw_application_new("com.raaz.servctl", G_APPLICATION_DEFAULT_FLAGS);

  g_action_map_add_action_entries(G_ACTION_MAP(app), app_actions,
                                  G_N_ELEMENTS(app_actions), app);

  g_signal_connect(app, "activate", G_CALLBACK(on_activate), NULL);

  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
