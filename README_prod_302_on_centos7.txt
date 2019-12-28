
********************************************************************************
*** build wireshark : packages
********************************************************************************
sudo yum install install git

sudo yum -y install epel-release
sudo yum install cmake3

sudo yum install libgcrypt-devel
sudo yum install glib2-devel

sudo yum install qt5-qttools
sudo yum install qt5-qtbase-devel
sudo yum install qt5-qttools-devel
sudo yum install libpcap-devel
sudo yum install qt5-qtmultimedia-devel
sudo yum install qt5-qtsvg-devel

sudo yum install zlib-devel

sudo yum install -y harfbuzz-devel.x86_64


********************************************************************************
*** build wireshark
********************************************************************************
mkdir wireshark
cd    wireshark

mkdir dev
cd    dev

git  clone  https://code.wireshark.org/review/wireshark  git-302X--linux64

cd  git-302X--linux64
git checkout v3.2.0

cmake3 .
make
--> run/wireshark


********************************************************************************
*** build wsgd
********************************************************************************
cd  plugins/epan
git clone https://gitlab.com/wsgd/wsgd.git  generic

cd  generic
cp -p  CMakeLists.300X.Linux.txt  CMakeLists.txt
cp -p  cmake_wireshark_version_number.cmake.example  cmake_wireshark_version_number.cmake
Edit cmake_wireshark_version_number.cmake to set 30200


cd  ../../..
Edit CMakeLists.txt to add a generic line just before the gryphon line

cmake3 .
make
--> run/wireshark
--> run/plugins/3.2/epan/generic.so
