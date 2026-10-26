install_data(
  files(
    'objetivo.png',
    'teg_icono.png',
    'robot.png',
    'color_player_green.png',
    'color_player_red.png',
    'color_player_black.png',
    'color_player_pink.png',
    'color_player_yellow.png',
    'color_player_blue.png',
    'disc_black.png',
    'disc_blue.png',
    'disc_green.png',
    'disc_pink.png',
    'disc_red.png',
    'disc_yellow.png',
    'disc_grey.png',
    'disc_over.png',
  ),
  install_dir: get_option('datadir') / 'teg',
)

# AppStream exports 64x64 icons by default and Flatpak requires square icons,
# so the hicolor icon is a square 64x64 derivative of teg_icono.png.
install_data('teg_icono_64.png',
  install_dir: get_option('datadir') / 'icons' / 'hicolor' / '64x64' / 'apps',
  rename: 'net.sf.teg.png',
)
