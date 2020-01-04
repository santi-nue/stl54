
********************************************************************************
*** Ubuntu 18.04
*** build wireshark : packages
********************************************************************************
sudo apt-get update
sudo apt-get --assume-yes install git

sudo apt-get --assume-yes install cmake
sudo apt-get --assume-yes install g++
sudo apt-get --assume-yes install libgtk2.0-dev
sudo apt-get --assume-yes install libgcrypt-dev
sudo apt-get --assume-yes install flex
sudo apt-get --assume-yes install bison

sudo apt-get --assume-yes install qtdeclarative5-dev
sudo apt-get --assume-yes install qttools5-dev-tools
sudo apt-get --assume-yes install qttools5-dev
sudo apt-get --assume-yes install qtmultimedia5-dev
sudo apt-get --assume-yes install libqt5svg5-dev


********************************************************************************
*** CentOS7
*** build wireshark : packages
********************************************************************************
sudo yum --assumeyes install git

sudo yum --assumeyes install epel-release
sudo yum --assumeyes install cmake3

sudo yum --assumeyes install libgcrypt-devel
sudo yum --assumeyes install glib2-devel

sudo yum --assumeyes install qt5-qttools
sudo yum --assumeyes install qt5-qtbase-devel
sudo yum --assumeyes install qt5-qttools-devel
sudo yum --assumeyes install libpcap-devel
sudo yum --assumeyes install qt5-qtmultimedia-devel
sudo yum --assumeyes install qt5-qtsvg-devel

sudo yum --assumeyes install zlib-devel
sudo yum --assumeyes install harfbuzz-devel.x86_64

alias cmake="cmake3"


********************************************************************************
*** openSUSE Leap 15-1
*** build wireshark : packages
********************************************************************************
sudo zypper --non-interactive in git

sudo zypper --non-interactive in cmake
sudo zypper --non-interactive in glib2-devel
sudo zypper --non-interactive in libgcrypt-devel
sudo zypper --non-interactive in flex
sudo zypper --non-interactive in bison

sudo zypper --non-interactive in libQt5Core-devel
sudo zypper --non-interactive in libqt5-linguist-devel
sudo zypper --non-interactive in libqt5-qttools-devel
sudo zypper --non-interactive in libqt5-qtmultimedia-devel
sudo zypper --non-interactive in libQt5PrintSupport-devel
sudo zypper --non-interactive in libqt5-qtsvg-devel



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

cmake .
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

cmake .
make
--> run/wireshark
--> run/plugins/3.2/epan/generic.so
