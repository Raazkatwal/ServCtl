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

executable(
  'servctl',
  sources: [
    'src/main.c'
  ],
  dependencies: [
    gtk_dep
  ],
  install: true
)
