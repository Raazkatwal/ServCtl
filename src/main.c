#include "servctl-application.h"

int main(int argc, char *argv[]) {
  return g_application_run(G_APPLICATION(servctl_application_new()), argc,
                           argv);
}
