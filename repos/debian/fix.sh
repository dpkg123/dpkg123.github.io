#!/bin/bash

set -eu

mkdir tmp
mv $(find -name *.deb) tmp
reprepro includedeb stable tmp/*
du -hs tmp
rm -rf tmp
