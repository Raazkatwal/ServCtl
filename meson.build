project(
  'servctl',
  'c',
  version: '0.1.0',
  default_options: ['warning_level=3', 'c_std=c11'],
)

gtk_dep = dependency('gtk4')
adw_dep = dependency('libadwaita-1')
gnome = import('gnome')

servctl_resources = gnome.compile_resources(
  'servctl-resources',
  'resources/servctl.gresource.xml',
  source_dir: 'resources',
  c_name: 'servctl',
)

gnome.compile_schemas(depend_files: files('data/servctl.gschema.xml'))

executable(
  'servctl',
  sources: [
    'src/main.c',
    'src/servctl-application.c',
    'src/servctl-window.c',
    'src/servctl-add-window.c',
    'src/services.c',
    'src/utils.c',
    servctl_resources,
  ],
  dependencies: [gtk_dep, adw_dep],
  install: true,
)

install_data(
  'data/servctl.gschema.xml',
  install_dir: get_option('datadir') / 'glib-2.0/schemas',
)
install_subdir('resources', install_dir: get_option('prefix') + '/share/servctl')
install_subdir('data', install_dir: get_option('prefix') + '/share/servctl')
