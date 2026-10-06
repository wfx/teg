# Installation

## Build dependencies

The build system is [Meson](https://mesonbuild.com/), using the Ninja backend.
In addition to the development packages of the used libraries you need
`meson` and `ninja`.

=== "Debian"

    ``` sh
    apt-get update && \
        DEBIAN_FRONTEND=noninteractive apt-get install --yes \
                build-essential debhelper gettext pkg-config \
                libgoocanvas-2.0-dev libxml2-dev \
                xmlto libgmock-dev python3-pip && \
        python3 -m pip install --upgrade meson ninja
    ```

=== "SuSE"

    ``` sh
    zypper --non-interactive  update && \
        zypper --non-interactive install -y gcc10-c++ \
        gettext gettext-tools make tidy gmock meson ninja \
        'pkgconfig(glib-2.0)' 'pkgconfig(libgnomeui-2.0)' 'pkgconfig(libxml-2.0)' \
        'perl(XML::Parser)' 'pkgconfig(goocanvas-2.0)' pkgconfig xmlto xz && \
        cd /usr/bin && ln -s gcc-10 gcc && ln -s g++-10 g++
    ```

## Building

The build is done out-of-source, so the source tree stays clean. Configure a
build directory and compile it:

``` sh
meson setup bd --prefix "$PWD/bd/DD"
meson compile -C bd
```

Run the unit tests with:

``` sh
meson test -C bd
```

## Installing

``` sh
meson install -C bd
```

After the installation, the binaries are located in `bd/DD/bin`, e.g. start the
client with `bd/DD/bin/tegclient`.

## Convenience script

The `./build` helper script performs all of the above (configure, build and
test) into the `bd` directory. Note that it erases the build directory first.

## Build options

Build options are set with `-D<option>=<value>` when configuring, for example
`-Dwerror=true` or `-Dtests=false`. The available options are described in
`meson_options.txt`.
