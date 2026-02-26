#include "window.h"
#include "glib-object.h"
#include <adwaita.h>
#include <gtk/gtk.h>

static void on_service_button_clicked(GtkButton *button, gpointer user_data) {
  const char *service = user_data;
  g_print("Toggled service: %s\n", service);
}

static GtkWidget *create_service_row(const char *service_name) {
  GtkWidget *row = adw_action_row_new();
	adw_action_row_set_subtitle(ADW_ACTION_ROW(row), service_name);

	GtkWidget *button = gtk_button_new_from_icon_name("media-playback-start-symbolic");

	gtk_widget_add_css_class(button, "flat");

	g_signal_connect(button, "clicked", G_CALLBACK(on_service_button_clicked), (gpointer) service_name);

  
	adw_action_row_add_suffix(ADW_ACTION_ROW(row), button);

  return row;
}

GtkWindow *servctl_window_new(AdwApplication *app) {
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *service_group;

  builder = gtk_builder_new_from_file("resources/ui/main.ui");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  service_group = GTK_WIDGET(gtk_builder_get_object(builder, "service_group"));

  const char *services[] = {"apache2", "nginx", "mysql", "postgresql", NULL};

  for (int i = 0; services[i]; i++) {
    GtkWidget *row = create_service_row(services[i]);
		adw_preferences_group_add(ADW_PREFERENCES_GROUP(service_group), row);
  }

  gtk_window_set_application(GTK_WINDOW(window), GTK_APPLICATION(app));


  g_object_unref(builder);

  return GTK_WINDOW(window);
}
