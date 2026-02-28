#pragma once

#include <adwaita.h>

#define SERVCTL_TYPE_APPLICATION (servctl_application_get_type())

G_DECLARE_FINAL_TYPE(ServctlApplication, servctl_application, SERVCTL,
                     APPLICATION, AdwApplication)

ServctlApplication *servctl_application_new(void);
