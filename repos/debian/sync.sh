#!/bin/bash
set -ex

TOKEN=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 OWNER=qier222 REPO=yesplaymusic ARCH=amd64 bash wget.sh
TOKEN=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 OWNER=qier222 REPO=yesplaymusic ARCH=arm64 bash wget.sh
TOKEN=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 OWNER=Voxelum REPO=x-minecraft-launcher ARCH=arm64 bash wget.sh
TOKEN=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 OWNER=Voxelum REPO=x-minecraft-launcher ARCH=amd64 bash wget.sh
TOKEN=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 OWNER=chrismaltby REPO=gb-studio bash wget.sh

reprepro includedeb stable *.deb
rm -rf *.deb
cd ~/test && bash deploy.sh
