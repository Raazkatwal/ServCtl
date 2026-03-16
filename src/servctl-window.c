#include "servctl-window.h"
#include "adwaita.h"
#include "gio/gio.h"
#include "glib-object.h"
#include "glib.h"
#include "gtk/gtkshortcut.h"
#include "services.h"
#include <gtk/gtk.h>

struct _ServctlWindow {
  AdwApplicationWindow parent_instance;

  AdwPreferencesGroup *service_group;

  GSettings *settings;
};

G_DEFINE_FINAL_TYPE(ServctlWindow, servctl_window, ADW_TYPE_APPLICATION_WINDOW)

static gboolean on_service_switch_toggled(GtkSwitch *sw, gboolean state, gpointer user_data) {
  const char *service_id = user_data;
  const char *action = state ? "start" : "stop";

  g_print("Toggling service %s: %s\n", service_id, action);

  GError *error = NULL;
  GSubprocess *proc = g_subprocess_new(
      G_SUBPROCESS_FLAGS_NONE, &error, "systemctl", action, service_id, NULL);

  if (!proc) {
    g_warning("Failed to run systemctl %s %s: %s", action, service_id, error->message);
    g_error_free(error);
    return TRUE; // Stop the switch from changing state
  }

  // Wait asynchronously to reap the child and avoid blocking GTK
  g_subprocess_wait_async(proc, NULL, NULL, NULL);
  g_object_unref(proc);

  return FALSE;
}

static GtkWidget *create_service_row(const char *service_entry) {
  g_auto(GStrv) parts = g_strsplit(service_entry, "|", 3);
  if (!parts || !parts[0] || !parts[1] || !parts[2]) {
    g_warning("Invalid service entry format: %s", service_entry);
    return NULL;
  }

  const char *service_id = parts[0];
  const char *display_name = parts[1];
  const char *status = parts[2];

  GtkWidget *row = adw_action_row_new();
  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), display_name);

  const char *subtitle = (g_strcmp0(status, "active") == 0) ? "Running" : "Stopped";
  adw_action_row_set_subtitle(ADW_ACTION_ROW(row), subtitle);

  GtkSwitch *sw = GTK_SWITCH(gtk_switch_new());
  gtk_widget_set_valign(GTK_WIDGET(sw), GTK_ALIGN_CENTER);

  gboolean active = (g_strcmp0(status, "active") == 0);
  gtk_switch_set_active(sw, active);

  // Store service_id to pass to the toggle callback. Use g_object_set_data_full for automatic cleanup
  g_object_set_data_full(G_OBJECT(sw), "service-id", g_strdup(service_id), g_free);
  g_signal_connect(sw, "state-set", G_CALLBACK(on_service_switch_toggled), 
                   g_object_get_data(G_OBJECT(sw), "service-id"));

  adw_action_row_add_suffix(ADW_ACTION_ROW(row), GTK_WIDGET(sw));

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
    if (row) {
      adw_preferences_group_add(self->service_group, row);
    }
  }

  g_strfreev(services);
}

static void servctl_window_init(ServctlWindow *self) {
  self->settings = g_settings_new("com.raaz.servctl");

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
