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
  [
    'main.cpp',
    'src/gui.cpp',
    'src/order.cpp',
    'src/socket.cpp',
    'src/wifibot.cpp',
  ],
  dependencies: [dependency('gtkmm-4.0')],
)
