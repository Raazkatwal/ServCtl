#include "services.h"
#include "utils.h"
#include <gio/gio.h>
#include <glib.h>

static void services_list_thread(GTask *task, gpointer source,
                                 gpointer task_data,
                                 GCancellable *cancellable) {
  g_autoptr(GError) error = NULL;
  g_autoptr(GSettings) settings = g_settings_new("com.raaz.servctl");

  g_auto(GStrv) services = g_settings_get_strv(settings, "services");

  GPtrArray *array = g_ptr_array_new_with_free_func(g_free);

  for (int i = 0; services[i] != NULL; i++) {

    const char *service = services[i];

    g_autoptr(GSubprocess) proc =
        g_subprocess_new(G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "systemctl",
                         "is-active", service, NULL);

    if (!proc) {
      g_task_return_error(task, g_steal_pointer(&error));
      return;
    }

    gchar *stdout_buf = NULL;

    if (!g_subprocess_communicate_utf8(proc, NULL, NULL, &stdout_buf, NULL,
                                       &error)) {
      g_free(stdout_buf);
      g_task_return_error(task, g_steal_pointer(&error));
      return;
    }

    gchar *status = g_strstrip(stdout_buf);

    gchar *name = NULL;
    if (g_str_has_suffix(service, ".service"))
      name = g_strndup(service, strlen(service) - 8);
    else
      name = g_strdup(service);

    gchar *display = format_service_name(name);
    g_free(name);

    gchar *entry = g_strdup_printf("%s|%s|%s", service, display, status);

    g_free(display);
    g_free(stdout_buf);

    g_ptr_array_add(array, entry);
  }

  g_ptr_array_add(array, NULL);

  g_task_return_pointer(task, g_ptr_array_free(array, FALSE),
                        (GDestroyNotify)g_strfreev);
}

void services_list_async(GAsyncReadyCallback callback, gpointer user_data) {
  GTask *task = g_task_new(NULL, NULL, callback, user_data);
  g_task_run_in_thread(task, services_list_thread);
  g_object_unref(task);
}

char **services_list_finish(GAsyncResult *res, GError **error) {
  return g_task_propagate_pointer(G_TASK(res), error);
}

static void services_list_all_thread(GTask *task, gpointer source,
                                     gpointer task_data,
                                     GCancellable *cancellable) {
  g_autoptr(GError) error = NULL;

  g_autoptr(GSubprocess) proc = g_subprocess_new(
      G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "systemctl", "list-unit-files",
      "--type=service", "--no-legend", "--all", "-q", NULL);

  if (!proc) {
    g_task_return_error(task, g_steal_pointer(&error));
    return;
  }

  gchar *stdout_buf = NULL;
  if (!g_subprocess_communicate_utf8(proc, NULL, NULL, &stdout_buf, NULL, &error)) {
    g_task_return_error(task, g_steal_pointer(&error));
    return;
  }

  g_auto(GStrv) lines = g_strsplit(stdout_buf, "\n", -1);
  GPtrArray *array = g_ptr_array_new_with_free_func(g_free);

  for (int i = 0; lines[i] && lines[i][0]; i++) {
    // The first word is the service name.
    g_auto(GStrv) parts = g_strsplit(lines[i], " ", 2);
    if (parts && parts[0]) {
      g_ptr_array_add(array, g_strdup(parts[0]));
    }
  }

  g_ptr_array_add(array, NULL);
  g_free(stdout_buf);

  g_task_return_pointer(task, g_ptr_array_free(array, FALSE), (GDestroyNotify)g_strfreev);
}

void services_list_all_async(GAsyncReadyCallback callback, gpointer user_data) {
  GTask *task = g_task_new(NULL, NULL, callback, user_data);
  g_task_run_in_thread(task, services_list_all_thread);
  g_object_unref(task);
}

char **services_list_all_finish(GAsyncResult *res, GError **error) {
  return g_task_propagate_pointer(G_TASK(res), error);
}
