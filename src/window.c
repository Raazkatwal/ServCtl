#include "window.h"
#include <gtk/gtk.h>

static void on_service_button_clicked(GtkButton *button, gpointer user_data) {
  const char *service = user_data;
  g_print("Toggled service: %s\n", service);
}

static GtkWidget *create_service_row(const char *service_name) {
  GtkWidget *row;
  GtkWidget *label;
  GtkWidget *button;

  row = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
	gtk_widget_set_margin_top(row, 6);
	gtk_widget_set_margin_bottom(row, 6);
	gtk_widget_set_margin_start(row, 12);
	gtk_widget_set_margin_end(row, 12);

  label = gtk_label_new(service_name);
  gtk_widget_set_hexpand(label, TRUE);
  gtk_widget_set_halign(label, GTK_ALIGN_START);

  button = gtk_button_new_from_icon_name("media-playback-start-symbolic");
  g_signal_connect(button, "clicked", G_CALLBACK(on_service_button_clicked),
                   (gpointer)service_name);

  gtk_box_append(GTK_BOX(row), label);
  gtk_box_append(GTK_BOX(row), button);

  return row;
}

GtkWindow *servctl_window_new(GtkApplication *app) {
  GtkBuilder *builder;
  GtkWidget *window;
  GtkWidget *service_list;

  builder = gtk_builder_new_from_file("resources/ui/main.ui");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  service_list = GTK_WIDGET(gtk_builder_get_object(builder, "service_list"));

  const char *services[] = {"apache2", "nginx", "mysql", "postgresql", NULL};

  for (int i = 0; services[i]; i++) {
    GtkWidget *row = create_service_row(services[i]);
    gtk_list_box_append(GTK_LIST_BOX(service_list), row);
  }

  gtk_window_set_application(GTK_WINDOW(window), app);

  GtkCssProvider *css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider, "resources/style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(css_provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_unref(builder);
  g_object_unref(css_provider);

  return GTK_WINDOW(window);
}
