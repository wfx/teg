project('teg', ['c', 'cpp'],
  version: '0.13.0',
  license: 'GPL-2.0-or-later',
  meson_version: '>= 0.60.0',
  default_options: [
    'cpp_std=gnu++17',
    'warning_level=0',
    'buildtype=debugoptimized',
  ])

# ---------------------------------------------------------------------------
# Global compiler configuration
# ---------------------------------------------------------------------------
cc = meson.get_compiler('c')
cxx = meson.get_compiler('cpp')

add_project_arguments('-DHAVE_CONFIG_H', language: ['c', 'cpp'])
# The code base is not warning-clean yet, so the built-in warning levels are
# deliberately left at 0: they would apply -Wall/-Wextra to C++ as well, which
# the current sources do not survive. -Werror is still honoured through the
# built-in "werror" option.
add_project_arguments('-fsigned-char', '-Wno-unused-parameter',
                      language: ['c', 'cpp'])

if get_option('debug_mode') == 'yes'
  add_project_arguments('-DDEBUG', language: ['c', 'cpp'])
elif get_option('debug_mode') == 'no'
  add_project_arguments('-DNDEBUG', language: ['c', 'cpp'])
endif

# ---------------------------------------------------------------------------
# Dependencies
# ---------------------------------------------------------------------------
glib_dep = dependency('glib-2.0')
xml_dep = dependency('libxml-2.0')
goocanvas_dep = dependency('goocanvas-2.0')
threads_dep = dependency('threads')

# readline is optional; the server falls back to a plain stdin reader
server_extra_deps = []
have_readline = false
readline_opt = get_option('readline')
if readline_opt.allowed()
  readline_dep = cc.find_library('readline', required: false)
  have_readline = readline_dep.found() and cc.has_header('readline/readline.h', required: false)
  if not have_readline and readline_opt.enabled()
    error('readline was requested, but libreadline or its header was not found.')
  endif
  if have_readline
    server_extra_deps += readline_dep
  endif
endif

# ---------------------------------------------------------------------------
# config.h
# ---------------------------------------------------------------------------
localedir = get_option('prefix') / get_option('localedir')

conf = configuration_data()
conf.set('VERSION', meson.project_version())
conf.set('LOCALEDIR', localedir)
conf.set('ENABLE_NLS', true)
conf.set('HAVE_LOCALE_H', cc.has_header('locale.h', required: false))
conf.set('HAVE_UNISTD_H', cc.has_header('unistd.h', required: false))
conf.set('HAVE_LIBREADLINE', have_readline)
conf.set('HAVE_PTHREADS', threads_dep.found())

configure_file(input: 'config.h.in', output: 'config.h', configuration: conf)

# ---------------------------------------------------------------------------
# Shared include directories (source + matching build directory)
# ---------------------------------------------------------------------------
top_inc = include_directories('.', 'common', 'client')

# ---------------------------------------------------------------------------
# Meson modules
# ---------------------------------------------------------------------------
gnome = import('gnome')
i18n = import('i18n')

# ---------------------------------------------------------------------------
# Subdirectories
# ---------------------------------------------------------------------------
subdir('common')
subdir('server')
subdir('client')
subdir('robot')
subdir('po')

# ---------------------------------------------------------------------------
# Unit tests
# ---------------------------------------------------------------------------
if get_option('tests')
  gtest_deps = []
  foreach gtest_name : ['gtest', 'gmock', 'gmock_main']
    gtest_dep = dependency(gtest_name, required: false)
    if not gtest_dep.found()
      gtest_dep = cxx.find_library(gtest_name, required: false)
    endif
    if not gtest_dep.found()
      error('Tests are enabled, but "' + gtest_name + '" was not found. Install the gtest/gmock development packages or configure with -Dtests=false.')
    endif
    gtest_deps += gtest_dep
  endforeach

  tegtest = executable('tegtest',
    files(
      'common/xml_support_check.cpp',
      'common/tarjeta_check.cpp',
      'common/stats_check.cpp',
      'common/share_check.cpp',
      'common/scores_check.cpp',
      'common/parser_check.cpp',
      'common/common_check.cpp',
      'client/helper_check.cpp',
      'client/gui-null/null.cpp',
    ),
    include_directories: top_inc,
    link_with: [libclient, libtegcommon],
    dependencies: [glib_dep, xml_dep] + gtest_deps,
  )
  test('tegtest', tegtest)

  tegservertest = executable('tegservertest',
    files('server/main.cpp', 'server/helper_check.cpp', 'server/player_check.cpp'),
    cpp_args: ['-DTEGSERVER_OMIT_MAIN=1'],
    include_directories: top_inc,
    link_with: [libtegserver, libtegcommon],
    dependencies: [glib_dep, xml_dep, threads_dep] + gtest_deps,
  )
  test('tegservertest', tegservertest)
endif

# Update the GSettings schema cache in the installation prefix after install.
gnome.post_install(glib_compile_schemas: true)
