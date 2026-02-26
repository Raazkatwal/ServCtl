#ifndef SERVCTL_SERVICES_H
#define SERVCTL_SERVICES_H

char **servctl_get_services(void);

void servctl_free_services(char **services);

#endif
