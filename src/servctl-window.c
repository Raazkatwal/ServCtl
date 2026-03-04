#include "servctl-window.h"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtkshortcut.h"
#include "services.h"
#include <gtk/gtk.h>

struct _ServctlWindow {
  AdwApplicationWindow parent_instance;

  AdwPreferencesGroup *service_group;
};

G_DEFINE_FINAL_TYPE(ServctlWindow, servctl_window, ADW_TYPE_APPLICATION_WINDOW)

static void on_service_button_clicked(GtkButton *button, gpointer user_data) {
  const char *service = user_data;
  g_print("Toggled service: %s\n", service);
}

static GtkWidget *create_service_row(const char *service_name) {
  GtkWidget *row = adw_action_row_new();
  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), service_name);

  GtkWidget *button =
      gtk_button_new_from_icon_name("media-playback-start-symbolic");

  gtk_widget_add_css_class(button, "flat");
  gtk_widget_add_css_class(button, "circular");

  g_signal_connect(button, "clicked", G_CALLBACK(on_service_button_clicked),
                   (gpointer)service_name);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);
  gtk_box_append(GTK_BOX(box), button);

  adw_action_row_add_suffix(ADW_ACTION_ROW(row), box);

  return row;
}

static void on_services_loaded(GObject *source, GAsyncResult *res,
                               gpointer user_data) {
  ServctlWindow *self = user_data;
  GError *error = NULL;

  char **services = services_list_finish(res, &error);

  if (error) {
    g_warning("%s", error->message);
    g_error_free(error);
    return;
  }

  for (int i = 0; services[i]; i++) {
    GtkWidget *row = create_service_row(services[i]);
    adw_preferences_group_add(self->service_group, row);
  }

  g_strfreev(services);
}

static void servctl_window_init(ServctlWindow *self) {
  gtk_widget_init_template(GTK_WIDGET(self));

	services_list_async(on_services_loaded, self);
}

static void servctl_window_class_init(ServctlWindowClass *klass) {
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);

  gtk_widget_class_set_template_from_resource(
      widget_class, "/com/raaz/servctl/ui/servctl-window.ui");

  gtk_widget_class_bind_template_child(widget_class, ServctlWindow,
                                       service_group);
}

ServctlWindow *servctl_window_new(AdwApplication *app) {
  return g_object_new(SERVCTL_TYPE_WINDOW, "application", app, NULL);
}
