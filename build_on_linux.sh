
echo This script has been tested on :
echo "- Kali 2019.2 (wsl) & wireshark 2.4 to 3.2"
echo 


################################################################################
### Configuration
################################################################################

# Set os version
[ -z "${wsgd_os_bits}" ]  && wsgd_os_bits=64
[ -z "${wsgd_os_name}" ]  && wsgd_os_name=ubuntu    # ubuntu, kali, opensuse, centos
[ -z "${wsgd_os_wsl}" ]   && wsgd_os_wsl=""         # set wsl if run on Windows Subsystem for Linux

[ -z "${wsgd_os_install_packages}" ]  && wsgd_os_install_packages="yes"
[ X"$1" == X"--packages:yes" ]        && wsgd_os_install_packages="yes"
[ X"$1" == X"--packages:no" ]         && wsgd_os_install_packages="no"

# Set wirewhark version
[ -z "${wsgd_wireshark_branch}" ]         && wsgd_wireshark_branch=302XX
[ -z "${wsgd_wireshark_checkout_label}" ] && wsgd_wireshark_checkout_label=v3.2.0
[ -z "${wsgd_wireshark_plugin_version}" ] && wsgd_wireshark_plugin_version=3.2
[ -z "${wsgd_WIRESHARK_VERSION_NUMBER}" ] && wsgd_WIRESHARK_VERSION_NUMBER=30200


# Set sources repositories
[ -z "${wsgd_wsgd_repository}" ]             && wsgd_wsgd_repository=https://gitlab.com/wsgd/wsgd.git
[ -z "${wsgd_wireshark_repository}" ]        && wsgd_wireshark_repository=https://code.wireshark.org/review/wireshark
[ -z "${wsgd_clone_wireshark_repository}" ]  && wsgd_clone_wireshark_repository="git  clone  ${wsgd_wireshark_repository}"


# Compute other configuration
[ -z "${wsgd_wireshark_src_subdir}" ]        && wsgd_wireshark_src_subdir=git-${wsgd_wireshark_branch}--linux${wsgd_os_bits}

[ -z "${wsgd_wsgd_CMakeLists}" ]             && wsgd_wsgd_CMakeLists=CMakeLists.${wsgd_wireshark_branch}.Linux.txt

if [ "${wsgd_wireshark_branch}" == "204XX" ]
then
	# No sub directory epan into plugins before 2.6
	[ -z "${wsgd_wireshark_src_plugin_subdir}" ] && wsgd_wireshark_src_plugin_subdir=plugins
	[ -z "${wsgd_wireshark_lib_plugin_subdir}" ] && wsgd_wireshark_lib_plugin_subdir=plugins
else
	[ -z "${wsgd_wireshark_src_plugin_subdir}" ] && wsgd_wireshark_src_plugin_subdir=plugins/epan
	[ -z "${wsgd_wireshark_lib_plugin_subdir}" ] && wsgd_wireshark_lib_plugin_subdir=plugins/${wsgd_wireshark_plugin_version}/epan
fi


################################################################################
### Configuration check
################################################################################

echo wsgd_os_bits = ${wsgd_os_bits}
echo wsgd_os_name = ${wsgd_os_name}
echo wsgd_os_wsl = ${wsgd_os_wsl}
echo wsgd_os_install_packages = ${wsgd_os_install_packages}

echo wsgd_wireshark_branch = ${wsgd_wireshark_branch}
echo wsgd_wireshark_checkout_label = ${wsgd_wireshark_checkout_label}
echo wsgd_wireshark_plugin_version = ${wsgd_wireshark_plugin_version}
echo wsgd_WIRESHARK_VERSION_NUMBER = ${wsgd_WIRESHARK_VERSION_NUMBER}

echo wsgd_wsgd_repository = ${wsgd_wsgd_repository}
echo wsgd_wireshark_repository = ${wsgd_wireshark_repository}
echo wsgd_clone_wireshark_repository = ${wsgd_clone_wireshark_repository}

echo wsgd_wireshark_src_subdir = ${wsgd_wireshark_src_subdir}
echo wsgd_wsgd_CMakeLists = ${wsgd_wsgd_CMakeLists}

echo wsgd_wireshark_src_plugin_subdir = ${wsgd_wireshark_src_plugin_subdir}
echo wsgd_wireshark_lib_plugin_subdir = ${wsgd_wireshark_lib_plugin_subdir}

echo 
echo wsgd: You must check/fix parameters displayed above
read -p "wsgd: Type Enter to continue ..."

################################################################################
### build wireshark : packages
################################################################################
if [ "${wsgd_os_install_packages}" == "yes" ]
then
	if [ "${wsgd_os_name}" == "ubuntu" ] || [ "${wsgd_os_name}" == "kali" ]
	then
		#-------------------------------------------------------------------------------
		#-- Ubuntu 18.04
		#-- Kali 2019.2
		#-------------------------------------------------------------------------------
		echo wsgd: install packages mandatory to build wireshark
		sudo apt-get update
		sudo apt-get --assume-yes install git

		sudo apt-get --assume-yes install cmake
		sudo apt-get --assume-yes install g++
		sudo apt-get --assume-yes install libgtk2.0-dev
		sudo apt-get --assume-yes install libgcrypt-dev
		sudo apt-get --assume-yes install flex
		sudo apt-get --assume-yes install bison

		sudo apt-get --assume-yes install qtdeclarative5-dev
		sudo apt-get --assume-yes install qttools5-dev
		sudo apt-get --assume-yes install qtmultimedia5-dev
		sudo apt-get --assume-yes install libqt5svg5-dev

		sudo apt-get --assume-yes install libpcap-dev


		if [ "${wsgd_os_name}" == "kali" ] && [ "${wsgd_os_wsl}" == "wsl" ]
		then
			echo "wsgd: fix make (and wireshark ...) will fail to find libQt5Core.so"
			sudo strip --remove-section=.note.ABI-tag /usr/lib/x86_64-linux-gnu/libQt5Core.so
		fi
	fi

	if [ "${wsgd_os_name}" == "centos" ]
	then
		#-------------------------------------------------------------------------------
		#-- CentOS7
		#-------------------------------------------------------------------------------
		echo wsgd: install packages mandatory to build wireshark
		sudo yum --assumeyes install git

		sudo yum --assumeyes install epel-release
		sudo yum --assumeyes install cmake3

		sudo yum --assumeyes install libgcrypt-devel
		sudo yum --assumeyes install glib2-devel

		sudo yum --assumeyes install qt5-qttools
		sudo yum --assumeyes install qt5-qtbase-devel
		sudo yum --assumeyes install qt5-qttools-devel
		sudo yum --assumeyes install qt5-qtmultimedia-devel
		sudo yum --assumeyes install qt5-qtsvg-devel

		sudo yum --assumeyes install libpcap-devel
		sudo yum --assumeyes install zlib-devel
		sudo yum --assumeyes install harfbuzz-devel.x86_64

		alias cmake="cmake3"
	fi

	if [ "${wsgd_os_name}" == "opensuse" ]
	then
		#-------------------------------------------------------------------------------
		#-- openSUSE Leap 15-1
		#-------------------------------------------------------------------------------
		echo wsgd: install packages mandatory to build wireshark
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

		sudo zypper --non-interactive in libpcap-devel
	fi
fi

################################################################################
### tools directory
################################################################################
wsgd_check_dir ()
{
	l_wsgd_dir_name=$1
	
	[ ! -d "${l_wsgd_dir_name}" ] && echo wsgd: Directory ${l_wsgd_dir_name} not found !    && exit 1
}

wsgd_cd ()
{
	l_wsgd_dir_name=$1
	
	wsgd_check_dir  ${l_wsgd_dir_name}
	cd  ${l_wsgd_dir_name}
	echo wsgd: $(pwd)
}

wsgd_cd_create_if_not_exist ()
{
	l_wsgd_dir_name=$1
	
	[ ! -d "${l_wsgd_dir_name}" ] && echo wsgd: Create directory ${l_wsgd_dir_name} && mkdir ${l_wsgd_dir_name}
	wsgd_cd  ${l_wsgd_dir_name}
}

################################################################################
### tools file
################################################################################
wsgd_check_exists ()
{
	l_wsgd_file_name=$1
	l_wsgd_output=$2
	
	[ ! -e "${l_wsgd_file_name}" ] && echo wsgd: File ${l_wsgd_file_name} not found !    && exit 1
	
	[ ! -z "${l_wsgd_output}" ] && echo wsgd: File ${l_wsgd_file_name} ${l_wsgd_output}
}

wsgd_check_executable ()
{
	l_wsgd_file_name=$1
	l_wsgd_output=$2
	
	wsgd_check_exists  ${l_wsgd_file_name}
	[ ! -x "${l_wsgd_file_name}" ] && echo wsgd: File ${l_wsgd_file_name} not executable !    && exit 1
	
	[ ! -z "${l_wsgd_output}" ] && echo wsgd: File ${l_wsgd_file_name} ${l_wsgd_output}
}

################################################################################
### tools 
################################################################################
wsgd_check_execution_ok ()
{
	l_wsgd_output=$1
	
	if [ $? -ne 0 ]
	then
		echo wsgd: Error ${l_wsgd_output}
		exit 1
	fi
	
	echo wsgd: ${l_wsgd_output} done
}


################################################################################
### build wireshark
################################################################################
wsgd_cd  ~
wsgd_cd_create_if_not_exist  wireshark
wsgd_cd_create_if_not_exist  dev

# Clone wireshark sources
if [ ! -d "${wsgd_wireshark_src_subdir}" ]
then
	echo wsgd: ${wsgd_clone_wireshark_repository}  ${wsgd_wireshark_src_subdir}
	${wsgd_clone_wireshark_repository}  ${wsgd_wireshark_src_subdir}
	wsgd_check_execution_ok  "wireshark clone"
	wsgd_check_dir  ${wsgd_wireshark_src_subdir}
fi
wsgd_cd  ${wsgd_wireshark_src_subdir}

wsgd_wireshark_src_dir=$(pwd)

# Checkout version
echo wsgd: git checkout ${wsgd_wireshark_checkout_label}
git checkout ${wsgd_wireshark_checkout_label}
wsgd_check_execution_ok  "wireshark checkout"

# Build
echo wsgd: wireshark cmake .
cmake .
wsgd_check_execution_ok  "wireshark cmake"

echo wsgd: wireshark make
make
wsgd_check_execution_ok  "wireshark make"

# Check wireshark
wsgd_check_executable  run/wireshark  "is present"


################################################################################
### build wsgd
################################################################################
wsgd_cd  ${wsgd_wireshark_src_dir}/${wsgd_wireshark_src_plugin_subdir}

# Clone wsgd sources
if [ ! -d generic ]
then
	echo wsgd: git clone ${wsgd_wsgd_repository}  generic
	git clone ${wsgd_wsgd_repository}  generic
	wsgd_check_execution_ok  "wsgd clone"
	wsgd_check_dir  generic 
fi

wsgd_cd  generic
wsgd_wsgd_src_dir=$(pwd)


if [ ! -e CMakeLists.txt ]
then
	echo wsgd: cp -p  ${wsgd_wsgd_CMakeLists}  CMakeLists.txt
	cp -p  ${wsgd_wsgd_CMakeLists}  CMakeLists.txt
	wsgd_check_exists  CMakeLists.txt
fi

if [ ! -e cmake_wireshark_version_number.cmake ]
then
	echo wsgd: Copy/edit wsgd cmake_wireshark_version_number.cmake
	cp -p  cmake_wireshark_version_number.cmake.example  cmake_wireshark_version_number.cmake
	wsgd_check_exists  cmake_wireshark_version_number.cmake
	sed -i s/-DWIRESHARK_VERSION_NUMBER=...../-DWIRESHARK_VERSION_NUMBER=${wsgd_WIRESHARK_VERSION_NUMBER}/  cmake_wireshark_version_number.cmake
fi


wsgd_cd  ${wsgd_wireshark_src_dir}


[ ! -e CMakeLists.txt ] && echo wsgd: File CMakeLists.txt not found !    && exit 1
grep  "${wsgd_wireshark_src_plugin_subdir}/generic"  CMakeLists.txt
if [ $? -ne 0 ]
then
    if [ ! -e CMakeLists.txt.generic ]
	then
		echo wsgd: Save CMakeLists.txt to CMakeLists.txt.generic
		cp -p CMakeLists.txt CMakeLists.txt.generic
		wsgd_check_exists  CMakeLists.txt.generic
	fi
	
	echo wsgd: Add ${wsgd_wireshark_src_plugin_subdir}/generic to wireshark CMakeLists.txt
	awk -v wsgd_wireshark_src_plugin_subdir=${wsgd_wireshark_src_plugin_subdir} '{ if (index($0, "gryphon") > 0) { printf("\t\t%s/generic\n", wsgd_wireshark_src_plugin_subdir); print; next } else { print; }}' CMakeLists.txt.generic > CMakeLists.txt

	grep  "${wsgd_wireshark_src_plugin_subdir}/generic"  CMakeLists.txt
	if [ $? -ne 0 ]
	then
		echo wsgd: Fail to add ${wsgd_wireshark_src_plugin_subdir}/generic to wireshark CMakeLists.txt
		exit 1
	fi
fi

# Build
echo wsgd: wireshark+wsgd cmake .
cmake .
wsgd_check_execution_ok  "wireshark+wsgd cmake"

echo wsgd: wireshark+wsgd make
make
wsgd_check_execution_ok  "wireshark+wsgd make"

# Check generic.so
# Check byte_interpret (not necessary for wireshark)
# Check unitary_tests (not necessary for wireshark)
wsgd_check_exists      run/${wsgd_wireshark_lib_plugin_subdir}/generic.so    "is present"
wsgd_check_executable  run/byte_interpret                                    "is present"
wsgd_check_executable  run/unitary_tests                                     "is present"


################################################################################
### unitary_tests
################################################################################
wsgd_cd  ${wsgd_wsgd_src_dir}

echo wsgd: ${wsgd_wireshark_src_dir}/run/unitary_tests
${wsgd_wireshark_src_dir}/run/unitary_tests
wsgd_check_execution_ok  "unitary_tests"


echo wsgd: Done
