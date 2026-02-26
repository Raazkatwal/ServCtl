#include <adwaita.h>
#include "app.h"

int main(int argc, char **argv)
{
	adw_init();
	return servctl_run(argc, argv);
}
