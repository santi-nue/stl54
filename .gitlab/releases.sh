#!/bin/sh
# sh because of image: registry.gitlab.com/gitlab-org/release-cli:v0.4.0

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

assets=""

for package in *
do
    echo "package=${package}"
    assets="${assets} --assets-link {\"name\":\"${package}\",\"url\":\"${PACKAGE_REGISTRY_URL}${package}\"}"
done

if [ -z "${assets}" ]
then
    echo "Aucun asset !"
    exit 3
fi

release-cli create --name "Release $CI_COMMIT_TAG" --tag-name $CI_COMMIT_TAG ${assets}

cd -
