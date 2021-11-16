#!/bin/sh
# sh because of image: curlimages/curl:latest

usage()
{
    echo "usage: $0  <packages sub dir>"
}

[ $# -lt 1 ] && echo "Missing arguments"  && usage && exit 1
[ $# -gt 1 ] && echo "Too much arguments" && usage && exit 2

packages_sub_dir=$1

cd ${packages_sub_dir}

pwd
ls

for package in *
do
    echo "package=${package}"
    curl --header "JOB-TOKEN: ${CI_JOB_TOKEN}" --upload-file ${package} ${PACKAGE_REGISTRY_URL}
    if [ $? -ne 0 ]
    then
        echo "  returns $?"
        exit 3
    fi
done

cd -
