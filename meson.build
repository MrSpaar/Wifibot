project(
  'wifibot',
  'cpp',
  version: '0.1.0',
)

gnome = import('gnome')
gresource = gnome.compile_resources('gresource', 'data/gresource.xml', source_dir: 'data')

executable(
  'bin',
  gresource,
  'src/main.cpp',
  dependencies: [dependency('gtkmm-4.0')],
)
