#pragma once
#include "gio/gio.h"
#include <glib.h>

void services_list_async(GAsyncReadyCallback callback, gpointer user_data);

char **services_list_finish(GAsyncResult *res, GError **error);

void service_toggle_async(const char *service, gboolean start,
                          GAsyncReadyCallback callback, gpointer user_data);

gboolean service_toggle_finish(GAsyncResult *res, GError **error);

char *services_get_status(const char *service);
