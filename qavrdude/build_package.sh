#!/bin/bash

qmake
make

cp -r package package_build
find  package_build \( -name ".svn" -o -name "*~" \) -exec rm -rf {} +
cp bin/qavrdude package_build/usr/bin/qavrdude
echo "Installed-Size:"`du -sx --exclude DEBIAN  package_build/ | sed -e 's/\([0-9]*\).*/\1/g'` >> package_build/DEBIAN/control
chmod -R 555 package_build/DEBIAN/*
dpkg-deb --build package_build qavrdude.deb
rm -rf package_build

