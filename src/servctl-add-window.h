#pragma once

#include <adwaita.h>

G_BEGIN_DECLS

#define SERVCTL_TYPE_ADD_WINDOW (servctl_add_window_get_type())

G_DECLARE_FINAL_TYPE(ServctlAddWindow, servctl_add_window, SERVCTL, ADD_WINDOW, AdwPreferencesWindow)

ServctlAddWindow *servctl_add_window_new(AdwApplication *app);

G_END_DECLS
