#!/bin/bash

# For wireshark >= 2.6
# Script for clone/build wireshark and clone/build wsgd
# sudo is used for install packages mandatory to build wireshark
# More precise script sequence is displayed when you launch it
 
# This script has been tested on :
# - CentOS 7.6         (wsl)    wireshark 2.4 to 3.6
# - CentOS 8.1         (wsl)    wireshark 3.0 to 3.6
# - CentOS stream 8    (wsl)    wireshark 3.0 to 4.0
# - CentOS stream 9    (wsl)    wireshark 4.0 to 4.2
# - Kali 2019.2        (wsl)    wireshark 2.4 to 3.6
# - openSUSE Leap 15-1 (wsl)    wireshark 2.4 to 3.6
# - Ubuntu 16.04       (wsl)    wireshark 3.2 to 3.4
# - Ubuntu 18.04       (wsl)    wireshark 3.0 to 3.6
# - Ubuntu 20.04       (wsl)    wireshark 3.0 to 3.6
# - Ubuntu 22.04       (wsl)    wireshark 3.6 to 4.2
# - Debian 10.10       (wsl)    wireshark 3.4 to 3.6

# Wsgd source code is now incompatible with wireshark <= 2.4
# Wsgd source code is partially c++11 :
# - could be incompatible with gcc before 4.8.1 (oldest version tested is 4.8.5)
# - could be incompatible with Visual C++ 2013 and below (no more tested)

################################################################################
### tools
################################################################################
wsgd__echo ()
{
	echo wsgd: $*
	return  0
}

wsgd__check_execution_ok ()
{
	if [ $? -ne 0 ]
	then
		wsgd__echo "Error $1"
		exit 1
	fi
	
	wsgd__echo "$1 done"
}

wsgd__prompt_continue ()
{
	[ "${wsgd_non_interactive}" == "yes" ] && echo $* && return 0

	read -p "wsgd: Type Enter to $*"
	return  0
}

#-------------------------------------------------------------------------------
#-- tools directory
#-------------------------------------------------------------------------------
wsgd__check_dir ()
{
	l_wsgd_dir_name=$1
	
	[ ! -d "${l_wsgd_dir_name}" ] && wsgd__echo "Directory ${l_wsgd_dir_name} not found !"    && exit 1
}

wsgd__cd ()
{
	l_wsgd_dir_name=$1
	
	wsgd__check_dir  ${l_wsgd_dir_name}
	cd  ${l_wsgd_dir_name}
	wsgd__echo "$(pwd)"
}

wsgd__cd_create_if_not_exist ()
{
	l_wsgd_dir_name=$1
	
	[ ! -d "${l_wsgd_dir_name}" ] && wsgd__echo "Create directory ${l_wsgd_dir_name}" && mkdir -p ${l_wsgd_dir_name}
	wsgd__cd  ${l_wsgd_dir_name}
}

#-------------------------------------------------------------------------------
#-- tools file
#-------------------------------------------------------------------------------
wsgd__check_exists ()
{
	l_wsgd_file_name=$1
	l_wsgd_output=$2
	
	[ ! -e "${l_wsgd_file_name}" ] && wsgd__echo "File ${l_wsgd_file_name} not found !"    && exit 1
	
	[ ! -z "${l_wsgd_output}" ] && wsgd__echo "File ${l_wsgd_file_name} ${l_wsgd_output}"
}

wsgd__check_executable ()
{
	l_wsgd_file_name=$1
	l_wsgd_output=$2
	
	wsgd__check_exists  ${l_wsgd_file_name}
	[ ! -x "${l_wsgd_file_name}" ] && wsgd__echo "File ${l_wsgd_file_name} not executable !"    && exit 1
	
	[ ! -z "${l_wsgd_output}" ] && wsgd__echo "File ${l_wsgd_file_name} ${l_wsgd_output}"
}


################################################################################
### Options
################################################################################
[ -z "${wsgd_sudo}" ]  && wsgd_sudo=sudo
[ -z "${wsgd_strip}" ] && wsgd_strip=strip
[ -z "${wsgd_test}" ]  && wsgd_test="yes"
wsgd_delivery_dir=""

while (( $# > 0 ))
do
	case $1 in
		"--packages:no")
			wsgd_os_install_packages="no"
			;;
		"--packages:yes")
			wsgd_os_install_packages="yes"
			;;
		"--no-sudo")
			wsgd_sudo=""
			;;
		"--non-interactive")
			wsgd_non_interactive="yes"
			;;
		"--fast")
			wsgd_build_fast="yes"
			;;
		"--test:no")
			wsgd_test="no"
			;;
		"--strip:no")
			wsgd_strip=""
			;;
		"--delivery_dir")
			shift
			(( $# < 1 )) && echo "Missing delivery directory" && exit 2
			wsgd_delivery_dir=$1
			;;
		*)
			wsgd__echo "Unknow option $1"
			exit 1
			;;
	esac
	shift
done

################################################################################
### Configuration
################################################################################

# Set os version
if [ -z "${wsgd_os_bits}" ]
then
	wsgd_os_bits=64
	uname -a | grep -i x86_64
	[ $? -ne 0 ] && wsgd_os_bits=32
fi
[ -z "${wsgd_os_name}" ]  && wsgd_os_name=$(cat /etc/os-release | grep "^ID=" | sed -e "s/ID=//" | sed -e 's/"//g')
if [ -z "${wsgd_os_wsl}" ]
then
	wsgd_os_wsl="native"
	uname -a | grep -i Microsoft
	[ $? -eq 0 ] && wsgd_os_wsl="wsl"       # Windows Subsystem for Linux
fi
[ -z "${wsgd_os_like_name}" ]  && wsgd_os_like_name=$(cat /etc/os-release | grep "^ID_LIKE=" | sed -e "s/ID_LIKE=//" | sed -e 's/"//g')
[ -z "${wsgd_os_like_name}" ]  && wsgd_os_like_name=${wsgd_os_name}

[ -z "${wsgd_os_install_packages}" ]  && wsgd_os_install_packages="yes"

# Set wirewhark version
[ -z "${wsgd_wireshark_branch}" ]         && wsgd_wireshark_branch=402XX
[ -z "${wsgd_wireshark_plugin_version}" ] && wsgd_wireshark_plugin_version=4.2
[ -z "${wsgd_wireshark_checkout_label}" ] && wsgd_wireshark_checkout_label=v4.2.0
[ -z "${wsgd_WIRESHARK_VERSION_NUMBER}" ] && wsgd_WIRESHARK_VERSION_NUMBER=40200


# Set sources repositories
[ -z "${wsgd_wsgd_repository}" ]             && wsgd_wsgd_repository=https://gitlab.com/wsgd/wsgd.git
[ -z "${wsgd_wireshark_repository}" ]        && wsgd_wireshark_repository=https://gitlab.com/wireshark/wireshark.git
[ -z "${wsgd_clone_wireshark_repository}" ]  && wsgd_clone_wireshark_repository="git  clone  --no-checkout  ${wsgd_wireshark_repository}"

# Directory where to clone
[  -z "${wsgd_dev_base_dir}" ]         && wsgd_dev_base_dir=~/wireshark/dev

# Compute other configuration
[ -z "${wsgd_wireshark_src_subdir}" ]        && wsgd_wireshark_src_subdir=git-${wsgd_wireshark_branch}--linux${wsgd_os_bits}

[ -z "${wsgd_wsgd_CMakeLists}" ]             && wsgd_wsgd_CMakeLists=CMakeLists.${wsgd_wireshark_branch}.txt
[ -z "${wsgd_cmake_build_dir}" ]             && wsgd_cmake_build_dir=${wsgd_dev_base_dir}/${wsgd_wireshark_src_subdir}

[ -z "${wsgd_wireshark_src_plugin_subdir}" ] && wsgd_wireshark_src_plugin_subdir=plugins/epan
[ -z "${wsgd_wireshark_lib_plugin_subdir}" ] && wsgd_wireshark_lib_plugin_subdir=plugins/${wsgd_wireshark_plugin_version}/epan

################################################################################
### Script sequence
################################################################################
install_packages_disabled="DISABLED: "
[ "${wsgd_os_install_packages}" == "yes" ] && install_packages_disabled=""

echo "For wireshark >= 2.6"
echo
echo "Script sequence (specific values depends on configuration) :"
echo "- ${install_packages_disabled}install every package mandatory to build wireshark"
echo "- cd  ${wsgd_dev_base_dir}                        create it if does not exist"
echo
echo "- if  ${wsgd_wireshark_src_subdir}  does not exist"
echo "  - ${wsgd_clone_wireshark_repository}   ${wsgd_wireshark_src_subdir}"
echo "  - git checkout  ${wsgd_wireshark_checkout_label}"
if [ "${wsgd_build_fast}" == "yes" ]
then
	echo "- copy of libw...so into ${wsgd_cmake_build_dir}/run"
else
	echo "- cmake -S .  -B ${wsgd_cmake_build_dir}"
	echo "- make"
fi
echo
echo "- cd  plugins/epan"
echo "- if  generic  does not exist"
echo "  - git clone ${wsgd_wsgd_repository}  generic"
echo "- configure generic directory"
echo "- cd  ../.."
echo "- add ${wsgd_wireshark_src_plugin_subdir}/generic into CMakeListsCustom.txt"
echo "- cmake -S .  -B ${wsgd_cmake_build_dir}"
if [ "${wsgd_build_fast}" == "yes" ]
then
	echo "- make generic/fast byte_interpret/fast unitary_tests/fast"
else
	echo "- make"
fi
echo
if [ "${wsgd_test}" == "yes" ]
then
	echo "- launch wsgd unitary_tests"
	echo "- launch wsgd tests_tshark"
	echo
fi
wsgd__prompt_continue "display configuration ..."
echo


################################################################################
### Configuration check
################################################################################

echo wsgd_os_bits = ${wsgd_os_bits}
echo wsgd_os_name = ${wsgd_os_name}
echo wsgd_os_like_name = ${wsgd_os_like_name}
echo wsgd_os_wsl = ${wsgd_os_wsl}
echo wsgd_os_install_packages = ${wsgd_os_install_packages}

echo wsgd_wireshark_branch = ${wsgd_wireshark_branch}
echo wsgd_wireshark_checkout_label = ${wsgd_wireshark_checkout_label}
echo wsgd_wireshark_plugin_version = ${wsgd_wireshark_plugin_version}
echo wsgd_WIRESHARK_VERSION_NUMBER = ${wsgd_WIRESHARK_VERSION_NUMBER}

echo wsgd_wsgd_repository = ${wsgd_wsgd_repository}
echo wsgd_wireshark_repository = ${wsgd_wireshark_repository}
echo wsgd_clone_wireshark_repository = ${wsgd_clone_wireshark_repository}

echo wsgd_dev_base_dir = ${wsgd_dev_base_dir}

echo wsgd_wireshark_src_subdir = ${wsgd_wireshark_src_subdir}
echo wsgd_wsgd_CMakeLists = ${wsgd_wsgd_CMakeLists}
echo wsgd_cmake_build_dir = ${wsgd_cmake_build_dir}

echo wsgd_wireshark_src_plugin_subdir = ${wsgd_wireshark_src_plugin_subdir}
echo wsgd_wireshark_lib_plugin_subdir = ${wsgd_wireshark_lib_plugin_subdir}

echo 
wsgd__echo "You must check parameters displayed above"
wsgd__echo "If they are not good, stop the script and fix them"
wsgd__prompt_continue "continue ..."

################################################################################
### build wireshark : packages
################################################################################
wsgd_cmake=cmake

if [ "${wsgd_os_install_packages}" == "yes" ]
then
	if type apt-get 2>/dev/null
	then
		#-------------------------------------------------------------------------------
		#-- Debian 10.10
		#-- Ubuntu 16.04 						                3.6 needs Qt 5.6
		#-- Ubuntu 18.04 						                4.0 needs pcre2
		#-- Ubuntu 20.04                                        4.2 needs Qt 6
		#-- Ubuntu 22.04
		#-- Kali 2019.2
		#-------------------------------------------------------------------------------
		wsgd__echo "install packages mandatory to build wireshark"
		${wsgd_sudo} apt-get update
		${wsgd_sudo} apt-get --assume-yes install git

		${wsgd_sudo} apt-get --assume-yes install cmake
		${wsgd_sudo} apt-get --assume-yes install g++
		${wsgd_sudo} apt-get --assume-yes install libgtk2.0-dev
		${wsgd_sudo} apt-get --assume-yes install libgcrypt-dev
		${wsgd_sudo} apt-get --assume-yes install flex
		${wsgd_sudo} apt-get --assume-yes install bison
		${wsgd_sudo} apt-get --assume-yes install libc-ares-dev

		if (( $wsgd_WIRESHARK_VERSION_NUMBER >= 40200 ))
		then
			${wsgd_sudo} apt-get --assume-yes install qt6-base-dev
			${wsgd_sudo} apt-get --assume-yes install qt6-multimedia-dev
			${wsgd_sudo} apt-get --assume-yes install qt6-tools-dev
			${wsgd_sudo} apt-get --assume-yes install qt6-tools-dev-tools
			${wsgd_sudo} apt-get --assume-yes install qt6-l10n-tools
			${wsgd_sudo} apt-get --assume-yes install libqt6core5compat6-dev
			# ${wsgd_sudo} apt-get --assume-yes install freeglut3-dev
			# ${wsgd_sudo} apt-get --assume-yes install libvulkan-dev
			# ${wsgd_sudo} apt-get --assume-yes install libxkbcommon-dev
		else
			${wsgd_sudo} apt-get --assume-yes install qtdeclarative5-dev
			${wsgd_sudo} apt-get --assume-yes install qttools5-dev
			${wsgd_sudo} apt-get --assume-yes install qtmultimedia5-dev
			${wsgd_sudo} apt-get --assume-yes install libqt5svg5-dev
		fi
		
		if (( $wsgd_WIRESHARK_VERSION_NUMBER >= 40200 ))
		then
			${wsgd_sudo} apt-get --assume-yes install libspeexdsp-dev
		fi
		
		${wsgd_sudo} apt-get --assume-yes install libpcap-dev


		if [ "${wsgd_os_wsl}" == "wsl" ]
		then
			# Problem seen on Kali 2019.2 & Ubuntu 20.04
			wsgd__echo "fix make (and wireshark ...) will fail to find libQt5Core.so"
			${wsgd_sudo} strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt5Core.so
			# Problem seen on Ubuntu 22.04
			wsgd__echo "fix make will fail to find libQt6Core.so.6"
			${wsgd_sudo} strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt6Core.so.6
		fi

	elif type yum 2>/dev/null
	then
		#-------------------------------------------------------------------------------
		#-- CentOS7         eol: 2024-06-30						4.0 needs gcrypt 1.8.0
		#-- CentOS8         eol: 2021-12-31						4.0 needs pcre2
		#-- CentOS Stream 8 eol: 2024-05-31						libpcap-devel not found : PowerTools
		#														radiotap-gen compilation error : pcap.h not found into wspcap.h
		#														4.2 needs qt6
		#-- CentOS Stream 9 eol: ~2027
		#-------------------------------------------------------------------------------
		wsgd__echo "install packages mandatory to build wireshark"
		${wsgd_sudo} yum --assumeyes install git

		${wsgd_sudo} yum --assumeyes install epel-release
		${wsgd_sudo} yum --assumeyes install cmake3 libarchive
		${wsgd_sudo} yum --assumeyes install make
		${wsgd_sudo} yum --assumeyes install python3
		${wsgd_sudo} yum --assumeyes install perl
		${wsgd_sudo} yum --assumeyes install gcc-c++

		${wsgd_sudo} yum --assumeyes install libgcrypt-devel
		${wsgd_sudo} yum --assumeyes install glib2-devel
		${wsgd_sudo} yum --assumeyes install flex
		${wsgd_sudo} yum --assumeyes install bison
		${wsgd_sudo} yum --assumeyes install c-ares-devel

		if (( $wsgd_WIRESHARK_VERSION_NUMBER >= 40000 ))
		then
			${wsgd_sudo} yum --assumeyes install pcre2-devel
		fi

		if (( $wsgd_WIRESHARK_VERSION_NUMBER >= 40200 ))
		then
			${wsgd_sudo} yum --assumeyes install qt6-qttools
			${wsgd_sudo} yum --assumeyes install qt6-qtbase-devel
			${wsgd_sudo} yum --assumeyes install qt6-qttools-devel
			${wsgd_sudo} yum --assumeyes install qt6-qtmultimedia-devel
			${wsgd_sudo} yum --assumeyes install qt6-qtsvg-devel
			${wsgd_sudo} yum --assumeyes install qt6-qt5compat-devel
		else
			${wsgd_sudo} yum --assumeyes install qt5-qttools
			${wsgd_sudo} yum --assumeyes install qt5-qtbase-devel
			${wsgd_sudo} yum --assumeyes install qt5-qttools-devel
			${wsgd_sudo} yum --assumeyes install qt5-qtmultimedia-devel
			${wsgd_sudo} yum --assumeyes install qt5-qtsvg-devel
		fi

		if (( $wsgd_WIRESHARK_VERSION_NUMBER >= 40200 ))
		then
			# yum did not find speexdsp-devel on CentOS Stream 9
			${wsgd_sudo} dnf --enablerepo=crb install speexdsp-devel
		fi

		${wsgd_sudo} yum --assumeyes install libpcap-devel
		${wsgd_sudo} yum --assumeyes install zlib-devel
		${wsgd_sudo} yum --assumeyes install harfbuzz-devel.x86_64

		if [ "${wsgd_os_wsl}" == "wsl" ]
		then
			# Problem seen on CentOS Stream 9
			wsgd__echo "fix make will fail to find libQt6Core.so"
			${wsgd_sudo} strip --remove-section=.note.ABI-tag /usr/lib64/libQt6Core.so.6
		fi

		wsgd_cmake=cmake3

	elif type zypper 2>/dev/null
	then
		#-------------------------------------------------------------------------------
		#-- openSUSE Leap 15-1 				                    4.2 needs cmake 3.13
		#-------------------------------------------------------------------------------
		wsgd__echo "install packages mandatory to build wireshark"
		${wsgd_sudo} zypper --non-interactive in git

		${wsgd_sudo} zypper --non-interactive in cmake
		${wsgd_sudo} zypper --non-interactive in gcc-c++
		${wsgd_sudo} zypper --non-interactive in glib2-devel
		${wsgd_sudo} zypper --non-interactive in libgcrypt-devel
		${wsgd_sudo} zypper --non-interactive in flex
		${wsgd_sudo} zypper --non-interactive in bison
		${wsgd_sudo} zypper --non-interactive in c-ares-devel

		${wsgd_sudo} zypper --non-interactive in libQt5Core-devel
		${wsgd_sudo} zypper --non-interactive in libqt5-linguist-devel
		${wsgd_sudo} zypper --non-interactive in libqt5-qttools-devel
		${wsgd_sudo} zypper --non-interactive in libqt5-qtmultimedia-devel
		${wsgd_sudo} zypper --non-interactive in libQt5PrintSupport-devel
		${wsgd_sudo} zypper --non-interactive in libqt5-qtsvg-devel
		
		${wsgd_sudo} zypper --non-interactive in libpcap-devel
		
	else
		wsgd__echo "apt-get, yum and zypper are not found"
		wsgd__echo "One must be found to install packages"
		wsgd__echo "So NO package has been installed"
		wsgd__prompt_continue "continue ..."
	fi
fi

# Check/search cmake
! type ${wsgd_cmake} 2>/dev/null && wsgd_cmake=""
[ -z ${wsgd_cmake} ] && type cmake3 2>/dev/null  && wsgd_cmake=cmake3
[ -z ${wsgd_cmake} ] && type cmake  2>/dev/null  && wsgd_cmake=cmake
[ -z ${wsgd_cmake} ] && wsgd__echo "cmake/cmake3 not found"


################################################################################
### clone & cmake wireshark
################################################################################
wsgd__cd_create_if_not_exist  ${wsgd_dev_base_dir}

# Clone wireshark sources
if [ ! -d "${wsgd_wireshark_src_subdir}" ]
then
	wsgd__echo "${wsgd_clone_wireshark_repository}  ${wsgd_wireshark_src_subdir}"
	${wsgd_clone_wireshark_repository}  ${wsgd_wireshark_src_subdir}
	wsgd__check_execution_ok  "wireshark clone"
	wsgd__check_dir  ${wsgd_wireshark_src_subdir}

	wsgd__cd  ${wsgd_wireshark_src_subdir}

	# Checkout version
	wsgd__echo "git checkout ${wsgd_wireshark_checkout_label}"
	git checkout ${wsgd_wireshark_checkout_label}
	wsgd__check_execution_ok  "wireshark checkout"
else
	wsgd__cd  ${wsgd_wireshark_src_subdir}
fi

wsgd_wireshark_src_dir=$(pwd)


# cmake (3.11 does not have -S -B options)
wsgd__cd ${wsgd_cmake_build_dir}
wsgd__echo "wireshark ${wsgd_cmake}  ${wsgd_wireshark_src_dir}"
${wsgd_cmake}  ${wsgd_wireshark_src_dir}
wsgd__check_execution_ok  "wireshark ${wsgd_cmake}"


################################################################################
### build wireshark
################################################################################
wsgd__cd ${wsgd_wireshark_src_dir}

if [ "${wsgd_build_fast}" == "yes" ]
then
	wireshark_prebuild_file=../wireshark_run/${wsgd_os_name}/${wsgd_wireshark_branch}/libw.tgz
	[ ! -r "${wireshark_prebuild_file}" ] && wireshark_prebuild_file=../wireshark_run/${wsgd_os_like_name}/${wsgd_wireshark_branch}/libw.tgz

	if [ ! -r "${wireshark_prebuild_file}" ]
	then
		# Prebuild file not found, so need to build wireshark
		wsgd_build_fast="no"
	else
		wsgd__echo "No wireshark build : creation of run with libw*.so*"
		mkdir -p ${wsgd_cmake_build_dir}/run && tar xzvf "${wireshark_prebuild_file}" -C ${wsgd_cmake_build_dir}/run
		ls -l ${wsgd_cmake_build_dir}/run

		# CMake Error at /home/olivier/wireshark/dev/git-304XX_02/CMakeFiles/CMakeTmp/CMakeLists.txt:14 (add_executable):
		#  Cannot find source file:
		#    /home/olivier/wireshark/dev/git-304XX_02/cmake/TestFileOffsetBits.c
		# rm -f $(find . -name "*.c")
	fi
fi

if [ "${wsgd_build_fast}" != "yes" ]
then
	# Build
	wsgd__cd  ${wsgd_cmake_build_dir}
	wsgd__echo "wireshark make"
	make
	wsgd__check_execution_ok  "wireshark make"

	# Check wireshark
	wsgd__check_executable  ${wsgd_cmake_build_dir}/run/wireshark  "is present"
fi


################################################################################
### build wsgd
################################################################################
wsgd__cd  ${wsgd_wireshark_src_dir}/${wsgd_wireshark_src_plugin_subdir}

# Clone wsgd sources
if [ ! -d generic ]
then
	wsgd__echo "git clone ${wsgd_wsgd_repository}  generic"
	git clone ${wsgd_wsgd_repository}  generic
	wsgd__check_execution_ok  "wsgd clone"
	wsgd__check_dir  generic 
fi

wsgd__cd  generic
wsgd_wsgd_src_dir=$(pwd)

# Add CMakeLists.txt if not already done
if [ ! -e CMakeLists.txt ]
then
	wsgd__echo "cp -p  ${wsgd_wsgd_CMakeLists}  CMakeLists.txt"
	cp -p  ${wsgd_wsgd_CMakeLists}  CMakeLists.txt
	wsgd__check_exists  CMakeLists.txt
fi

# Add cmake_wireshark_version_number.cmake if not already done
if [ ! -e cmake_wireshark_version_number.cmake ]
then
	wsgd__echo "Copy/edit wsgd cmake_wireshark_version_number.cmake"
	cp -p  cmake_wireshark_version_number.cmake.example  cmake_wireshark_version_number.cmake
	wsgd__check_exists  cmake_wireshark_version_number.cmake
	sed -i s/-DWIRESHARK_VERSION_NUMBER=...../-DWIRESHARK_VERSION_NUMBER=${wsgd_WIRESHARK_VERSION_NUMBER}/  cmake_wireshark_version_number.cmake
fi


wsgd__cd  ${wsgd_wireshark_src_dir}


# Add CMakeListsCustom.txt if not already done
if [ ! -e CMakeListsCustom.txt ]
then
	wsgd__check_exists  CMakeListsCustom.txt.example

	wsgd__echo "cp -p  CMakeListsCustom.txt.example  CMakeListsCustom.txt"
	cp -p  CMakeListsCustom.txt.example  CMakeListsCustom.txt
fi

wsgd__check_exists  CMakeListsCustom.txt

# Check if generic is into CMakeListsCustom.txt
grep  "${wsgd_wireshark_src_plugin_subdir}/generic"  CMakeListsCustom.txt
if [ $? -ne 0 ]
then
	wsgd__echo "Save CMakeListsCustom.txt to CMakeListsCustom.txt.generic"
	cp -p CMakeListsCustom.txt CMakeListsCustom.txt.generic
	wsgd__check_exists  CMakeListsCustom.txt.generic
	
	wsgd__echo "Add ${wsgd_wireshark_src_plugin_subdir}/generic to wireshark CMakeListsCustom.txt"
	awk -v wsgd_wireshark_src_plugin_subdir=${wsgd_wireshark_src_plugin_subdir} '{ if (index($0, "plugins/epan/foo") > 0) { print; printf("\t%s/generic\n", wsgd_wireshark_src_plugin_subdir); next } else { print; }}' CMakeListsCustom.txt.generic > CMakeListsCustom.txt

	grep  "${wsgd_wireshark_src_plugin_subdir}/generic"  CMakeListsCustom.txt
	if [ $? -ne 0 ]
	then
		wsgd__echo "Fail to add ${wsgd_wireshark_src_plugin_subdir}/generic to wireshark CMakeListsCustom.txt"
		exit 1
	fi
fi

# Build
wsgd__cd ${wsgd_cmake_build_dir}
wsgd__echo "wireshark+wsgd ${wsgd_cmake}  ${wsgd_wireshark_src_dir}"
${wsgd_cmake}  ${wsgd_wireshark_src_dir}
wsgd__check_execution_ok  "wireshark+wsgd ${wsgd_cmake}"

wsgd__cd  ${wsgd_cmake_build_dir}
if [ "${wsgd_build_fast}" == "yes" ]
then
	wsgd__echo "wireshark+wsgd make generic/fast byte_interpret/fast unitary_tests/fast"
	make libfdesc/fast generic/fast byte_interpret/fast unitary_tests/fast
	wsgd__check_execution_ok  "wireshark+wsgd make generic/fast byte_interpret/fast unitary_tests/fast"
else
	wsgd__echo "wireshark+wsgd make"
	make
	wsgd__check_execution_ok  "wireshark+wsgd make"
fi

# Check generic.so
# Check byte_interpret (not necessary for wireshark)
# Check unitary_tests (not necessary for wireshark)
wsgd__cd  ${wsgd_cmake_build_dir}
wsgd__check_exists      run/${wsgd_wireshark_lib_plugin_subdir}/generic.so    "is present"
wsgd__check_executable  run/byte_interpret                                    "is present"
wsgd__check_executable  run/unitary_tests                                     "is present"


################################################################################
### unitary_tests
### tests_tshark
################################################################################
wsgd__cd  ${wsgd_wsgd_src_dir}

if [ "${wsgd_test}" == "yes" ]
then
	wsgd__echo "${wsgd_cmake_build_dir}/run/unitary_tests"
	${wsgd_cmake_build_dir}/run/unitary_tests
	wsgd__check_execution_ok  "unitary_tests"

	wsgd__echo "./tests_tshark/test.sh"
	./tests_tshark/test.sh
	wsgd__check_execution_ok  "tests_tshark"
fi

################################################################################
### strip
################################################################################
if [ ! -z "${wsgd_strip}" ]
then
	${wsgd_strip}  ${wsgd_cmake_build_dir}/run/byte_interpret
	${wsgd_strip}  ${wsgd_cmake_build_dir}/run/${wsgd_wireshark_lib_plugin_subdir}/generic.so
fi

################################################################################
### delivery
################################################################################
if [ ! -z "${wsgd_delivery_dir}" ]
then
	wsgd__cd  ${wsgd_delivery_dir}
	cp -p  ${wsgd_cmake_build_dir}/run/byte_interpret  .
	cp -p  ${wsgd_cmake_build_dir}/run/${wsgd_wireshark_lib_plugin_subdir}/generic.so  .
	if [ "${wsgd_build_fast}" != "yes" ]
	then
		wsgd__cd  ${wsgd_cmake_build_dir}/run
		[ ! -z "${wsgd_strip}" ] && ${wsgd_strip}  libw*.so*
		tar cvzf ${wsgd_delivery_dir}/libw.tgz  libw*.so*
		[ ! -z "${wsgd_strip}" ] && ${wsgd_strip}  tshark
		tar cvzf ${wsgd_delivery_dir}/tshark.tgz  tshark
	fi
fi


wsgd__echo "Done"
