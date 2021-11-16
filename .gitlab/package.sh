#!/bin/bash

usage()
{
    echo "usage: $0  <file to generate>  <input file path>  [<other git dir> ...]"
}

(( $# < 2 )) && echo "Missing arguments"  && usage && exit 1

output_file_path=$1
shift

# input file (generic.so)
generated_input_file_path=$1
shift
[ ! -r "${generated_input_file_path}" ] && echo "${generated_input_file_path} is not readable" && usage && exit 2

generated_input_dir=$(dirname ${generated_input_file_path})
generated_input_file=$(basename ${generated_input_file_path})


strip ${generated_input_file_path}

$(dirname $0)/package_log.sh  ${generated_input_file_path}.txt  ${generated_input_file_path}  $*


tar czvf ${output_file_path}  ChangeLog.txt  -C ${generated_input_dir} ${generated_input_file} ${generated_input_file}.txt
