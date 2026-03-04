#include "services.h"
#include "gio/gio.h"
#include "glib-object.h"
#include <glib.h>

static void services_list_thread(GTask *task, gpointer source,
                                 gpointer task_data,
                                 GCancellable *cancellable) {
  GError *error = NULL;

  GSubprocess *proc = g_subprocess_new(
      G_SUBPROCESS_FLAGS_STDOUT_PIPE, &error, "systemctl", "list-units",
      "--type=service", "--no-legend", "--no-pager", NULL);

  if (!proc) {
    g_task_return_error(task, error);
    return;
  }

  gchar *stdout_buf = NULL;

  if (!g_subprocess_communicate_utf8(proc, NULL, NULL, &stdout_buf, NULL,
                                     &error)) {
    g_object_unref(proc);
    g_task_return_error(task, error);
    return;
  }

  GPtrArray *array = g_ptr_array_new_with_free_func(g_free);

  gchar **lines = g_strsplit(stdout_buf, "\n", -1);

  for (int i = 0; lines[i]; i++) {

    gchar *line = g_strstrip(lines[i]);

    if (strlen(line) == 0)
      continue;

    gchar **cols = g_strsplit(line, " ", 2);

    if (cols[0] && g_str_has_suffix(cols[0], ".service"))
      g_ptr_array_add(array, g_strdup(cols[0]));

    g_strfreev(cols);
  }

  g_strfreev(lines);
  g_free(stdout_buf);
  g_object_unref(proc);

  g_ptr_array_add(array, NULL);

  g_task_return_pointer(task, g_ptr_array_free(array, FALSE),
                        (GDestroyNotify)g_strfreev);
}

void services_list_async(GAsyncReadyCallback callback, gpointer user_data) {
  GTask *task = g_task_new(NULL, NULL, callback, user_data);
  g_task_run_in_thread(task, services_list_thread);
}

char **services_list_finish(GAsyncResult *res, GError **error) {
  return g_task_propagate_pointer(G_TASK(res), error);
}
