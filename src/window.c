#include "window.h"
#include <gtk/gtk.h>

GtkWindow *servctl_window_new(GtkApplication *app) {
  GtkBuilder *builder;
  GtkWidget *window;

  builder = gtk_builder_new_from_file("resources/ui/main.ui");

  window = GTK_WIDGET(gtk_builder_get_object(builder, "main_window"));

  gtk_window_set_application(GTK_WINDOW(window), app);

  GtkCssProvider *css_provider = gtk_css_provider_new();
  gtk_css_provider_load_from_path(css_provider, "resources/style.css");
  gtk_style_context_add_provider_for_display(gdk_display_get_default(),
                                             GTK_STYLE_PROVIDER(css_provider),
                                             GTK_STYLE_PROVIDER_PRIORITY_USER);

  g_object_unref(builder);

  return GTK_WINDOW(window);
}
