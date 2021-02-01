#!/bin/bash

docker run --rm -v $(pwd):/ccos4rbpi -w /ccos4rbpi -u $(id -u ) ccos4rbpi make $@ ARCH=arm64 BOARD=raspberry-pi-4
