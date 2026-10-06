# ---------------------------------------------------------------------------
# Stock icons, compiled from stock.gresource.xml into stock_gresources.{c,h}
#
# We drive glib-compile-resources directly (instead of gnome.compile_resources)
# to keep the exact upstream behaviour: the generated symbols are placed in the
# "stock" namespace and registered manually, which is what stock.cpp expects.
# ---------------------------------------------------------------------------
glib_compile_resources = find_program('glib-compile-resources')

gresource_xml = files('stock.gresource.xml')

stock_gresources_c = custom_target('stock_gresources.c',
  input: gresource_xml,
  output: 'stock_gresources.c',
  command: [
    glib_compile_resources,
    '--sourcedir=@0@'.format(meson.current_source_dir()),
    '--generate-source',
    '--manual-register',
    '--c-name=stock',
    '--target=@OUTPUT@',
    '@INPUT@',
  ],
)

stock_gresources_h = custom_target('stock_gresources.h',
  input: gresource_xml,
  output: 'stock_gresources.h',
  command: [
    glib_compile_resources,
    '--sourcedir=@0@'.format(meson.current_source_dir()),
    '--generate-header',
    '--manual-register',
    '--c-name=stock',
    '--target=@OUTPUT@',
    '@INPUT@',
  ],
)

libguiclient = static_library('teg_client_gui',
  files(
    'gui.cpp',
    'callbacks.cpp',
    'interface.cpp',
    'chatline.cpp',
    'priv.cpp',
    'support.cpp',
    'g_cont.cpp',
    'g_country.cpp',
    'stock.cpp',
    'cards.cpp',
    'dices.cpp',
    'mission.cpp',
    'status.cpp',
    'connect.cpp',
    'armies.cpp',
    'colors.cpp',
    'preferences.cpp',
    'g_scores.cpp',
    'locate_country.cpp',
    'tegcanvasimage.cpp',
  ),
  stock_gresources_c,
  stock_gresources_h,
  include_directories: [top_inc, include_directories('.')],
  c_args: ['-DG_LOG_DOMAIN="TEG"'],
  cpp_args: ['-DG_LOG_DOMAIN="TEG"'],
  dependencies: [goocanvas_dep, glib_dep, xml_dep],
)

# ---------------------------------------------------------------------------
# GSettings schema: compile it into the build tree (so the client can be run
# from there during development) and install it for system-wide use.
# ---------------------------------------------------------------------------
gnome.compile_schemas()

install_data('net.sf.teg.gschema.xml',
  install_dir: get_option('datadir') / 'glib-2.0' / 'schemas',
)

# ---------------------------------------------------------------------------
# Desktop integration data
# ---------------------------------------------------------------------------
install_data('teg.desktop',
  install_dir: get_option('datadir') / 'applications',
)
