# qrcoder
QR Code generator 

This app was written to develop large QR Codes for scanning at long ranges. It supports generation of QR Code images up to 10 meter (32.8 ft) (iPhone 6 and later) as well as multi-page printing for generating posters and banners which can be scanned at drive-by speeds.

## Prerequisites

QRCoder uses the following software packages:
- libqrencode -https://fukuchi.org/works/qrencode/
- Qt 5.x - https://www.qt.io/
- Windows - Microsoft VCPKG (Windows Only) - https://github.com/microsoft/vcpkg

### Building on Qt (all Platforms)
Open the qrcoder.pro under QTCreator and press the Green arrow to build. Of course you will need to install the libraries above before getting started.

### Building for Linux
Install QT5.x
Clone the tree and check out qrcoder
```
cd qrcoder
sudo apt-get install libqrencode-dev
mkdir build; cd build
cmake .. -DQRENCODE_LIBRARY_DEBUG=QRENCODE_LIBRARY
make
```

### Building for Windows
```
vcpkg install qt5 --triplet x64-windows
vcpkg install libqrencode --triplet x64-windows
vcpkg integrate install

mkdir build
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$(VCPKGDIR)/scripts/buildsystems/vcpkg.cmake
```
## Running qrcoder
![QRCode](https://raw.githubusercontent.com/jafrado/qrcoder/master/doc/qrcoder.png)
- qrcoder supports QRCodes that can scan up to 10 meter (32.8 ft) (iPhone 6 and later)
- Change the distance (rightmost input box) for the distance you would like to scan at
- Change the error coding level if you'd like to overlay an image in the middle, use Highest level of Error correction
- Save as PNG or JPG
- Print as a multi-page print and tape together for large targets at range



