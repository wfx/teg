# Flatpak

`net.sf.teg.yml` builds the T.E.G. client as a Flatpak application. The
manifest targets the GNOME runtime, so GTK 3 comes from the runtime and only
`goocanvas-2.0` has to be built as an extra module.

## Prerequisites

* `flatpak` and `flatpak-builder`
* the GNOME SDK matching the manifest:

  ```sh
  flatpak install flathub org.gnome.Sdk//50
  ```

## Building

The helper script builds the application and writes a bundle that can be
copied to other machines:

```sh
scripts/build-flatpak
```

The result is `bd-flatpak/net.sf.teg.flatpak` (ignored by git, like every
`bd-*` directory). Install it with:

```sh
flatpak install --user bd-flatpak/net.sf.teg.flatpak
flatpak run net.sf.teg
```

Use `--bundle PATH` to choose a different output file and `--install` to
install into the current user's Flatpak installation instead of writing a
bundle. The script passes `--disable-rofiles-fuse`, so it also works on build
machines without FUSE.

The Flatpak is built from the current working tree and uses the branch
`master`, i.e. the current development version. For a named release pass the
release name, which becomes the Flatpak branch and part of the bundle name:

```sh
scripts/build-flatpak --release 0.13.0
# writes bd-flatpak/net.sf.teg-0.13.0.flatpak
```

## Notes

* The sandbox needs network access, because the client talks to a game server
  over TCP. Files the game writes (`~/.local/share/teg`) end up in
  `~/.var/app/net.sf.teg/`.
* The client can start a local `tegserver` and `tegrobot`; both binaries are
  part of the bundle and are found next to `tegclient` in `/app/bin`.
* `-Dwerror=false` is used here because the GNOME SDK ships a newer
  gdk-pixbuf that deprecates the XPM API used by the GUI. CI keeps enforcing
  `-Dwerror=true` against Debian 13.
* For a Flathub submission the metadata still needs screenshots and an icon
  with a larger resolution; `net.sf.teg` is used as the app id because it is
  already the GSettings schema id.
