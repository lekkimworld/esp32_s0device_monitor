#!/bin/sh

ts=`date +%Y%m%d\T%H%M`

pio run -e dev -t clean
pio run -e dev
pio run -e dev -t buildfs
pio run -e prod -t clean
pio run -e prod
pio run -e prod -t buildfs

rm -rf builds/$ts
mkdir -p builds/$ts/dev
mkdir -p builds/$ts/prod
cp .pio/build/dev/*.bin       builds/$ts/dev
cp .pio/build/prod/*.bin      builds/$ts/prod

git add builds/$ts
git commit -m "Built release $ts"
git tag $ts
