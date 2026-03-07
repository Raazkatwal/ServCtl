#include "utils.h"
#include "glib.h"
#include <string.h>

gchar *format_service_name(const char *service){
	gchar *name = g_strdup(service);

	if (g_str_has_suffix(name, ".service")) {
		name[strlen(name) - 8] = '\0';
	}

	for (gchar *p = name; *p; p++) {
		if (*p == '-') {
			*p = ' ';
		}
	}

	name[0] = g_ascii_toupper(name[0]);

	return name;
}
