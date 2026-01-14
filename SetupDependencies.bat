@echo off
echo Setting up dependencies...
mkdir Intermediate\Downloads

echo Downloading SDL3...
curl -L https://github.com/libsdl-org/SDL/releases/download/release-3.2.8/SDL3-devel-3.2.8-VC.zip -o Intermediate\Downloads\SDL3-devel-3.2.8-VC.zip

mkdir ThirdParty\SDL3-3.2.8
tar -xf Intermediate\Downloads\SDL3-devel-3.2.8-VC.zip -C ThirdParty\SDL3-3.2.8 --strip-components=1