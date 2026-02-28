project(
  'servctl',
  'c',
  version: '0.1.0',
  default_options: [
    'warning_level=3',
    'c_std=c11'
  ]
)

gtk_dep = dependency('gtk4')
adw_dep = dependency('libadwaita-1')
gnome = import('gnome')

servctl_resources = gnome.compile_resources(
  'servctl-resources',
  'resources/servctl.gresource.xml',
  source_dir: 'resources',
  c_name: 'servctl'
)

executable('servctl',
  sources: [
    'src/main.c',
    'src/servctl-application.c',
    'src/servctl-window.c',
    servctl_resources
  ],
  dependencies: [
    dependency('gtk4'),
    dependency('libadwaita-1')
  ],
  install: true
)

install_subdir('resources', install_dir: get_option('prefix') + '/share/servctl')
install_subdir('data', install_dir: get_option('prefix') + '/share/servctl')
