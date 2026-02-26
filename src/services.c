#include "services.h"
#include <glib.h>
#include <string.h>

char **servctl_get_services(void) {
  gchar *stdout_data = NULL;
  gchar *stderr_data = NULL;
  gint exit_status;

  GError *error = NULL;

  if (!g_spawn_command_line_sync(
          "systemctl list-unit-files --type=service --no-pager --no-legend",
          &stdout_data, &stderr_data, &exit_status, &error)) {
    g_printerr("Failed to run systemctl: %s\n", error->message);
    g_error_free(error);
    return NULL;
  }

  if (!stdout_data) {
    return NULL;
  }

  gchar **lines = g_strsplit(stdout_data, "\n", -1);

  g_free(stdout_data);
  g_free(stderr_data);

  return lines;
}

void servctl_free_services(char **services) { g_strfreev(services); }
