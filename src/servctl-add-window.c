#include "servctl-add-window.h"
#include "services.h"
#include <glib.h>

struct _ServctlAddWindow {
  AdwPreferencesWindow parent_instance;

  AdwPreferencesGroup *all_services_group;
  GSettings *settings;
  GStrv active_services;
};

G_DEFINE_FINAL_TYPE(ServctlAddWindow, servctl_add_window, ADW_TYPE_PREFERENCES_WINDOW)

static void on_service_toggled(GtkCheckButton *check_button, gpointer user_data) {
  ServctlAddWindow *self = user_data;
  const char *service_id = g_object_get_data(G_OBJECT(check_button), "service-id");
  gboolean check_active = gtk_check_button_get_active(check_button);

  // Read current services array
  g_auto(GStrv) current_services = g_settings_get_strv(self->settings, "services");
  
  GPtrArray *array = g_ptr_array_new_with_free_func(g_free);
  gboolean found = FALSE;

  for (int i = 0; current_services && current_services[i]; i++) {
    if (g_strcmp0(current_services[i], service_id) == 0) {
      found = TRUE;
      if (check_active) {
        g_ptr_array_add(array, g_strdup(current_services[i]));
      }
    } else {
      g_ptr_array_add(array, g_strdup(current_services[i]));
    }
  }

  if (check_active && !found) {
    g_ptr_array_add(array, g_strdup(service_id));
  }

  g_ptr_array_add(array, NULL);

  // Update GSettings
  g_settings_set_strv(self->settings, "services", (const gchar *const *)array->pdata);
  g_ptr_array_unref(array);

  // Refresh caching
  g_clear_pointer(&self->active_services, g_strfreev);
  self->active_services = g_settings_get_strv(self->settings, "services");
}

static GtkWidget *create_all_service_row(ServctlAddWindow *self, const char *service_id) {
  GtkWidget *row = adw_action_row_new();
  adw_preferences_row_set_title(ADW_PREFERENCES_ROW(row), service_id);

  GtkWidget *check = gtk_check_button_new();
  gtk_widget_set_valign(check, GTK_ALIGN_CENTER);
  
  gboolean active = FALSE;
  if (self->active_services) {
    for (int i = 0; self->active_services[i] != NULL; i++) {
      if (g_strcmp0(self->active_services[i], service_id) == 0) {
        active = TRUE;
        break;
      }
    }
  }

  gtk_check_button_set_active(GTK_CHECK_BUTTON(check), active);
  g_object_set_data_full(G_OBJECT(check), "service-id", g_strdup(service_id), g_free);
  g_signal_connect(check, "toggled", G_CALLBACK(on_service_toggled), self);

  adw_action_row_add_suffix(ADW_ACTION_ROW(row), check);
  adw_action_row_set_activatable_widget(ADW_ACTION_ROW(row), check);

  return row;
}

static void on_all_services_loaded(GObject *source, GAsyncResult *res, gpointer user_data) {
  ServctlAddWindow *self = user_data;
  GError *error = NULL;

  char **services = services_list_all_finish(res, &error);

  if (error) {
    g_warning("Failed to load all services: %s", error->message);
    g_error_free(error);
    return;
  }

  for (int i = 0; services && services[i]; i++) {
    GtkWidget *row = create_all_service_row(self, services[i]);
    if (row) {
      adw_preferences_group_add(self->all_services_group, row);
    }
  }

  g_strfreev(services);
}

static void servctl_add_window_init(ServctlAddWindow *self) {
  self->settings = g_settings_new("com.raaz.servctl");
  self->active_services = g_settings_get_strv(self->settings, "services");

  gtk_widget_init_template(GTK_WIDGET(self));

  services_list_all_async(on_all_services_loaded, self);
}

static void servctl_add_window_dispose(GObject *object) {
  ServctlAddWindow *self = SERVCTL_ADD_WINDOW(object);
  g_clear_object(&self->settings);
  g_clear_pointer(&self->active_services, g_strfreev);
  G_OBJECT_CLASS(servctl_add_window_parent_class)->dispose(object);
}

static void servctl_add_window_class_init(ServctlAddWindowClass *klass) {
  GObjectClass *object_class = G_OBJECT_CLASS(klass);
  object_class->dispose = servctl_add_window_dispose;

  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS(klass);
  gtk_widget_class_set_template_from_resource(widget_class, "/com/raaz/servctl/ui/servctl-add-window.ui");
  gtk_widget_class_bind_template_child(widget_class, ServctlAddWindow, all_services_group);
}

ServctlAddWindow *servctl_add_window_new(AdwApplication *app) {
  return g_object_new(SERVCTL_TYPE_ADD_WINDOW, "application", app, NULL);
}
