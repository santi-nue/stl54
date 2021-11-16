#!/bin/bash

usage()
{
    echo "usage: $0  <output packages sub dir>  <input delivery sub dir>"
}

(( $# < 2 )) && echo "Missing arguments"  && usage && exit 1
(( $# > 2 )) && echo "Too much arguments" && usage && exit 2

packages_sub_dir=$1
delivery_sub_dir=$2

mkdir -p ${packages_sub_dir}

for version in ${VERSIONS}
do
    prefix=${packages_sub_dir}/${CI_COMMIT_TAG}-${version}
    $(dirname $0)/package.sh  ${prefix}-generic.tgz         ${delivery_sub_dir}/${version}/generic.so  wireshark
    $(dirname $0)/package.sh  ${prefix}-byte_interpret.tgz  ${delivery_sub_dir}/${version}/byte_interpret
    if [ -r ${delivery_sub_dir}/${version}/libw.tgz ]
    then
        cp -p  ${delivery_sub_dir}/${version}/libw.tgz   ${prefix}-libw.tgz
    fi
done
