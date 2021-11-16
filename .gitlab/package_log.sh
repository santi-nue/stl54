#!/bin/bash

usage()
{
    echo "usage: $0  <file to generate>  <input file path>  [<other git dir> ...]"
}

(( $# < 2 )) && echo "Missing arguments" && usage && exit 1

output_file_path=$1
shift

# input file (so or exe)
generated_input_file_path=$1
shift
[ ! -r "${generated_input_file_path}" ] && echo "${generated_input_file_path} is not readable" && usage && exit 2
generated_input_file=$(basename ${generated_input_file_path})


(
    # 4fab41a1f0e14e9124ea6a61e7bae42e95599495
    # git rev-parse HEAD
    # commit 4fab41a1f0e14e9124ea6a61e7bae42e95599495
    # git log | head -n 1 
    # 4fab41a1f0e14e9124ea6a61e7bae42e95599495 (HEAD, tag: wireshark-1.12.0, tag: v1.12.0) Build 1.12.0.
    echo "generic: git log --pretty=oneline --decorate | head -n 1"
    git log --pretty=oneline --decorate | head -n 1
    echo
    echo

    echo "os: uname -a"
    uname -a
    echo
    echo

    echo "os: cat /etc/*-release"
    cat /etc/*-release
    echo
    echo

    echo "compiler: g++ -v"
    g++ -v 
    echo
    echo

    echo "generic: readelf -d  ${generated_input_file} | grep NEEDED"
    readelf -d  ${generated_input_file_path} | grep NEEDED
    echo
    echo

    for other_git_dir in $*
    do
        dir_name=$(basename $other_git_dir)
        echo "${dir_name}: git log --pretty=oneline --decorate | head -n 1"
        git -C ${other_git_dir} log --pretty=oneline --decorate | head -n 1
        echo
        echo
    done
)  > ${output_file_path}  2>&1
