executable('tegrobot',
  files(
    'main.cpp',
    'gui.cpp',
    'ai.cpp',
    'ai_msg.cpp',
    'ai_misc.cpp',
    'ai_fichas.cpp',
  ),
  include_directories: top_inc,
  link_with: [libclient, libtegcommon],
  dependencies: [glib_dep, xml_dep],
  install: true,
)
