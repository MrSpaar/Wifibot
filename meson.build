project(
  'wifibot',
  'cpp',
  version: '0.1.0',
)

gnome = import('gnome')

executable(
  'bin',
  gnome.compile_resources('gresource', 'data/icons.gresource.xml', source_dir: 'data/icons'),
  'src/main.cpp',
  dependencies: [dependency('gtkmm-4.0')],
)
