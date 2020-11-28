#!/usr/bin/bash

# name: build.sh
# desc: Helper script to build teg

handle_exit() {
  if [ -f teg*test.log ]; then
    grep . teg*test.log
  else 
    echo "No *test.log found."
  fi

  echo "Cleanup..."
  rm -rf "${BUILD_DIR}"
  cd "${SCRIPT_DIR}"
  rm po/Makefile.in.in
  rm po/Makevars.template
  rm po/Rules-quot
  rm po/boldquot.sed
  rm po/en@boldquot.header
  rm po/en@quot.header
  rm po/insert-header.sin
  rm po/quot.sed
  rm po/remove-potcdate.sin
  echo "Cleanup... done"
  exit "${EXIT_CODE}"
}

teg_build(){
  (cd "${SCRIPT_DIR}" && bash -eu autogen.sh)
  mkdir "${BUILD_DIR}"
  cd "${BUILD_DIR}"
  # Note: Do we need this?
  echo '*' > .gitignore
  # Note: prefix?
  "${SCRIPT_DIR}/configure" --enable-warnings-as-errors --enable-maintainer-mode --prefix=${PWD}/DD "CFLAGS=-Wall -Wextra"
  # Exit immediately if a command exits with a non-zero status
  set +e
  make all check
  EXIT_CODE="$?"
  set -e
}

# ===========================================================================

set -o errexit  # script exit when a command fails ( add "... || true" to allow fail)
set -o nounset  # script exit when it use undeclared variables
# set -o xtrace   # trace for debugging
set -o pipefail # exit status of last command that throws a non-zero exit code

trap handle_exit 0 SIGHUP SIGINT SIGQUIT SIGABRT SIGTERM

# if we called the script via a symbolic link
declare -r SCRIPT_DIR=$(dirname "$(readlink -f "$0")")
# bd=builddir
declare -r BUILD_DIR="${SCRIPT_DIR}/bd"
declare EXIT_CODE=-1

# Main
teg_build
exit
