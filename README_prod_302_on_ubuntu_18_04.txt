
********************************************************************************
*** build wireshark : packages
********************************************************************************
sudo apt-get install git

sudo apt-get install cmake
sudo apt-get install g++
sudo apt-get install libgtk2.0-dev          # for glib2
sudo apt-get install libgcrypt-dev
sudo apt-get install flex                   # for lex
sudo apt-get install bison                  # for yacc

sudo apt-get install qtdeclarative5-dev
sudo apt-get install qttools5-dev-tools
sudo apt-get install qttools5-dev
sudo apt-get install qtmultimedia5-dev
sudo apt-get install libqt5svg5-dev


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
--> ~/.config/wireshark/plugins/generic.so
