#pragma once
#include <adwaita.h>

G_BEGIN_DECLS

#define SERVCTL_TYPE_WINDOW (servctl_window_get_type())
G_DECLARE_FINAL_TYPE(ServctlWindow, servctl_window, SERVCTL, WINDOW,
                     AdwApplicationWindow)

ServctlWindow *servctl_window_new(AdwApplication *app);

G_END_DECLS
