#!/bin/bash

set -euo pipefail

PGSRC="${PGSRC:-}"
PGSQL="${PGSQL:-}"

WORKDIR="${WORKDIR:-$(pwd)}"
PATCHDIR="${WORKDIR}/patches"
OUTDIR="${OUTDIR:-${WORKDIR}/parser_ext}"

die() {
	printf 'error: %s\n' "$*" >&2
	exit 1
}

usage() {
	cat <<EOF
usage:
  PGSRC=/path/to/postgres/ PGSQL=/path/to/pgsql/ ./bootstrap.sh

env:
  PGSRC      PostgreSQL source path (required)
  PGSQL      PostgreSQL install path (required)
  OUTDIR     out directory (default: ./parser_ext)
EOF
}

for arg in "$@"; do
	case "$arg" in
	-h | --help)
		usage
		exit 0
		;;
	*) die "unknown argument: $arg" ;;
	esac
done

mkdir -p "$OUTDIR"

mkdir -vp "${OUTDIR}/tools/"
cp -v "${PGSRC}/src/tools/gen_keywordlist.pl" "${OUTDIR}/tools/"
cp -v "${PGSRC}/src/tools/PerfectHash.pm" "${OUTDIR}/tools/"

cp -v "${PGSRC}/src/backend/parser/gram.y" "${OUTDIR}/my_gram.y"
patch -d "$OUTDIR" -p0 <"${PATCHDIR}/my_gram.y.patch"

cp -v "${PGSRC}/src/backend/parser/gramparse.h" "${OUTDIR}/my_gramparse.h"
patch -d "$OUTDIR" -p0 <"${PATCHDIR}/my_gramparse.h.patch"

cp -v "${PGSRC}/src/include/parser/kwlist.h" "${OUTDIR}/my_kwlist.h"

cp -v "${WORKDIR}/Makefile" "${OUTDIR}/"
cp -v "${WORKDIR}/parser_ext.c" "${OUTDIR}/"

echo "PGSQL=\"$(realpath ${PGSQL})\"" >"${OUTDIR}/.env"
echo "PGSRC=\"$(realpath ${PGSRC})\"" >>"${OUTDIR}/.env"
