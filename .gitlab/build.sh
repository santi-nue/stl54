
wsgd_dev_base_dir=$(pwd)
export wsgd_dev_base_dir

wsgd_wireshark_src_subdir=wireshark
export wsgd_wireshark_src_subdir

wsgd_wireshark_src_dir=${wsgd_dev_base_dir}/${wsgd_wireshark_src_subdir}

wsgd_wsgd_repository=${wsgd_dev_base_dir}
export wsgd_wsgd_repository

usage()
{
    echo "usage: $0  <delivery sub dir>"
}

(( $# < 1 )) && echo "Missing arguments"  && usage && exit 1
(( $# > 1 )) && echo "Too much arguments" && usage && exit 2

delivery_sub_dir=$1


for version in ${VERSIONS}
do
    echo ------------------------------------------------------------
    echo --- Start ${version}
    echo ------------------------------------------------------------
    if [ "${version}" == "306XX" ]
    then
        wsgd_wireshark_branch=306XX
        wsgd_wireshark_plugin_version=3.6
        wsgd_wireshark_checkout_label=v3.6.0
        wsgd_WIRESHARK_VERSION_NUMBER=30600
    elif [ "${version}" == "304XX" ]
    then
        wsgd_wireshark_branch=304XX
        wsgd_wireshark_plugin_version=3.4
        wsgd_wireshark_checkout_label=v3.4.0
        wsgd_WIRESHARK_VERSION_NUMBER=30400
    elif [ "${version}" == "302XX" ]
    then
        wsgd_wireshark_branch=302XX
        wsgd_wireshark_plugin_version=3.2
        wsgd_wireshark_checkout_label=v3.2.0
        wsgd_WIRESHARK_VERSION_NUMBER=30200
    elif [ "${version}" == "300XX" ]
    then
        wsgd_wireshark_branch=300XX
        wsgd_wireshark_plugin_version=3.0
        wsgd_wireshark_checkout_label=v3.0.7
        wsgd_WIRESHARK_VERSION_NUMBER=30007
    elif [ "${version}" == "206XX" ]
    then
        wsgd_wireshark_branch=206XX
        wsgd_wireshark_plugin_version=2.6
        wsgd_wireshark_checkout_label=v2.6.13
        wsgd_WIRESHARK_VERSION_NUMBER=20613
    else
        echo "Unknow version ${version}"
        exit 2
    fi

    export wsgd_wireshark_branch
    export wsgd_wireshark_plugin_version
    export wsgd_wireshark_checkout_label
    export wsgd_WIRESHARK_VERSION_NUMBER
    
    delivery_dir=${wsgd_dev_base_dir}/${delivery_sub_dir}/${version}
    mkdir -p ${delivery_dir}

    if [ -d "${wsgd_wireshark_src_dir}" ]
    then
        git -C ${wsgd_wireshark_src_dir} checkout ${wsgd_wireshark_checkout_label}
        (( $? != 0 )) && echo "checkout ${wsgd_wireshark_checkout_label} fail !" && exit 3

        rm  ${wsgd_wireshark_src_dir}/CMakeListsCustom.txt
        rm  ${wsgd_wireshark_src_dir}/plugins/epan/generic/CMakeLists.txt
        rm  ${wsgd_wireshark_src_dir}/plugins/epan/generic/cmake_wireshark_version_number.cmake
    fi

    # Build into another directory to avoid conflict
    export wsgd_cmake_build_dir=${wsgd_wireshark_src_dir}_build_${wsgd_wireshark_branch}
    mkdir -p ${wsgd_cmake_build_dir}

    # --fast             do not build wireshark if possible
    # --no-sudo          image does not have sudo
    #                    do not need it into gitlab ci
    # --packages:yes     even with wireshark dev image, not everything is always installed
    # --test:no          done by another stage
    ./build_on_linux.sh  --delivery_dir ${delivery_dir}  --fast  --non-interactive  --no-sudo  --packages:yes
    (( $? != 0 )) && echo "build_on_linux.sh for ${version} fail !" && exit 4

done

echo Done
