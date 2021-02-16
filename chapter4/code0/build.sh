#!/bin/bash
set -e

if [[ $1 == "clean" ]]; then
    docker run --rm -v $(pwd):/ccos4rbpi -w /ccos4rbpi -u $(id -u ) ccos4rbpi make clean
else
    docker run --rm -v $(pwd):/ccos4rbpi -w /ccos4rbpi -u $(id -u ) ccos4rbpi make config
    docker run --rm -v $(pwd):/ccos4rbpi -w /ccos4rbpi -u $(id -u ) ccos4rbpi make exec ARCH=arm64
    docker run --rm -v $(pwd):/ccos4rbpi -w /ccos4rbpi -u $(id -u ) ccos4rbpi make ARCH=arm64 BOARD=raspberry-pi-4
fi
