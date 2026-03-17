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

static void on_systemctl_finished(GObject *source_object, GAsyncResult *res, gpointer user_data) {
  GSubprocess *proc = G_SUBPROCESS(source_object);
  GtkButton *button = GTK_BUTTON(user_data);
  GError *error = NULL;

  gboolean wait_success = g_subprocess_wait_finish(proc, res, &error);

  if (!wait_success) {
    g_warning("systemctl wait failed: %s", error->message);
    g_error_free(error);
  } else if (!g_subprocess_get_successful(proc)) {
    g_warning("systemctl process failed or was cancelled.");
  } else {
    // Only toggle the state visually if the process ran successfully
    gboolean is_active = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "is-active"));
    is_active = !is_active;
    g_object_set_data(G_OBJECT(button), "is-active", GINT_TO_POINTER(is_active));
    
    if (is_active) {
      gtk_button_set_icon_name(button, "media-playback-stop-symbolic");
      gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Stop");
    } else {
      gtk_button_set_icon_name(button, "media-playback-start-symbolic");
      gtk_widget_set_tooltip_text(GTK_WIDGET(button), "Start");
    }

    GtkWidget *row = gtk_widget_get_ancestor(GTK_WIDGET(button), ADW_TYPE_ACTION_ROW);
    if (row) {
      adw_action_row_set_subtitle(ADW_ACTION_ROW(row), is_active ? "Running" : "Stopped");
    }
  }

  // Restore button sensitivity and clean up
  gtk_widget_set_sensitive(GTK_WIDGET(button), TRUE);
  g_object_unref(button);
}

static void execute_systemctl(GtkButton *button, const char *action) {
  const char *service_id = g_object_get_data(G_OBJECT(button), "service-id");

  g_print("Executing systemctl %s %s\n", action, service_id);

  GError *error = NULL;
  GSubprocess *proc = g_subprocess_new(
      G_SUBPROCESS_FLAGS_NONE, &error, "systemctl", action, service_id, NULL);

  if (!proc) {
    g_warning("Failed to run systemctl %s %s: %s", action, service_id, error->message);
    g_error_free(error);
    return;
  }

  // Disable the button while the process is running
  gtk_widget_set_sensitive(GTK_WIDGET(button), FALSE);

  // Wait asynchronously for the process to finish
  g_subprocess_wait_async(proc, NULL, on_systemctl_finished, g_object_ref(button));
  g_object_unref(proc);
}

static void on_service_toggle_clicked(GtkButton *button, gpointer user_data) {
  gboolean is_active = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "is-active"));
  const char *action = is_active ? "stop" : "start";

  // execute_systemctl now handles the state change on success
  execute_systemctl(button, action);
}

static void on_service_restart_clicked(GtkButton *button, gpointer user_data) {
  execute_systemctl(button, "restart");
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

  gboolean active = (g_strcmp0(status, "active") == 0);
  const char *subtitle = active ? "Running" : "Stopped";
  adw_action_row_set_subtitle(ADW_ACTION_ROW(row), subtitle);

  GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
  gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

  GtkWidget *btn_toggle = gtk_button_new_from_icon_name(
      active ? "media-playback-stop-symbolic" : "media-playback-start-symbolic");
  gtk_widget_add_css_class(btn_toggle, "flat");
  gtk_widget_add_css_class(btn_toggle, "circular");
  gtk_widget_set_tooltip_text(btn_toggle, active ? "Stop" : "Start");
  g_object_set_data_full(G_OBJECT(btn_toggle), "service-id", g_strdup(service_id), g_free);
  g_object_set_data(G_OBJECT(btn_toggle), "is-active", GINT_TO_POINTER(active));
  g_signal_connect(btn_toggle, "clicked", G_CALLBACK(on_service_toggle_clicked), NULL);

  GtkWidget *btn_restart = gtk_button_new_from_icon_name("view-refresh-symbolic");
  gtk_widget_add_css_class(btn_restart, "flat");
  gtk_widget_add_css_class(btn_restart, "circular");
  gtk_widget_set_tooltip_text(btn_restart, "Restart");
  g_object_set_data_full(G_OBJECT(btn_restart), "service-id", g_strdup(service_id), g_free);
  g_signal_connect(btn_restart, "clicked", G_CALLBACK(on_service_restart_clicked), NULL);

  gtk_box_append(GTK_BOX(box), btn_toggle);
  gtk_box_append(GTK_BOX(box), btn_restart);

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
