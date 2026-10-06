libclient = static_library('teg_client',
  files(
    'player.cpp',
    'misc.cpp',
    'inputfd.cpp',
    'outputfd.cpp',
    'helper.cpp',
    'fichas.cpp',
    'attack.cpp',
    'reagrupe.cpp',
    'ejer2.cpp',
    'canje.cpp',
    'themes.cpp',
  ),
  include_directories: top_inc,
  dependencies: [glib_dep, xml_dep],
)

subdir('gui-gnome')

executable('tegclient',
  'main.cpp',
  include_directories: top_inc,
  link_with: [libguiclient, libclient, libtegcommon],
  dependencies: [glib_dep, goocanvas_dep, xml_dep],
  install: true,
)

subdir('teg_pix')
subdir('themes')
