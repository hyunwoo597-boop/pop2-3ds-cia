#!/usr/bin/env bash
set -euo pipefail
cd "$(dirname "$0")"
docker run --rm -v "$PWD:/project" -w /project devkitpro/devkitarm:latest \
  bash -lc 'make -f Makefile.ctr -j2 && ls -lh prince-of-persia-2.cia prince-of-persia-2.3dsx'
