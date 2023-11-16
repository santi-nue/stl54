#!/bin/bash -e

wireshark=../../../../run/wireshark
tshark=../../../../run/tshark

windiff="/mnt/c/Program Files/Microsoft SDKs/Windows/v7.0/Bin/WinDiff.Exe"

user_diff=0
user_windiff=0
[ X"$1" == X"--diff" ]    && shift && user_diff=1
[ X"$1" == X"--windiff" ] && shift && user_windiff=1
[ X"$1" == X"--diff" ]    && shift && user_diff=1

cd $(dirname $0)

# ***************************************************************************
# Check/compute wsgd_wireshark_branch
# ***************************************************************************
if [ -z "${wsgd_wireshark_branch}" ]
then
    # Try compute wsgd_wireshark_branch (e.g. 400XX) from the current path
    pwd | grep "^.*/git-...XX--linux64/.*$" > /dev/null
    if (( $? == 0 ))
    then
        wsgd_wireshark_branch=$(pwd | sed -e "s:^.*/git-::" -e "s:--linux64/.*$::")
        # echo ${wsgd_wireshark_branch}
    fi

    if [ -z "${wsgd_wireshark_branch}" ]
    then
        echo wsgd_wireshark_branch is missing
        exit 1
    fi
fi


# ***************************************************************************
# Build versions list (from our version to lower)
# ***************************************************************************
function get_versions_from_to_lower()
{
    our_version=$1
    
    version_list=""
    for version in 204XX 206XX 300XX 302XX 304XX 306XX 400XX 402XX
    do
        # echo ${version}
        version_list="${version} ${version_list}"
        [ "${version}" == "${our_version}" ] && break
    done

    echo ${version_list}
}

version_list=$(get_versions_from_to_lower "${wsgd_wireshark_branch}")
# echo ${version_list}

# ***************************************************************************
# Test all *.pcap into dir
# ***************************************************************************
function tst_dir()
{
    dir=$1
    
    for infile in $(ls $dir/*.pcap)
    do
        tst_dir_file $dir $infile
    done
}

# ***************************************************************************
# Test infile (pcap file)
# ***************************************************************************
function tst_dir_file()
{
    dir=$1
    infile=$2
    # echo infile=$infile

    export WIRESHARK_GENERIC_DISSECTOR_DIR=${dir}
    export WIRESHARK_GENERIC_DISSECTOR_TRACES_DIR=${dir}

    tst_out_full_file=${infile}.tst_out.full.txt
    tst_out_file=${infile}.tst_out.txt
    tst_err_full_file=${infile}.tst_err.full.txt
    tst_err_file=${infile}.tst_err.txt

    # Compute ref_out_file
    for version in ${version_list}
    do
        ref_out_file=${infile}.ref_out.${version}.txt
        [ -e "${ref_out_file}" ] && break
    done
    if [ ! -e "${ref_out_file}" ]
    then
        echo ERROR: ref_out_file=${ref_out_file} not found
        exit 1
    fi
    # echo ref_out_file=${ref_out_file}

    # Compute ref_err_file
    for version in ${version_list}
    do
        ref_err_file=${infile}.ref_err.${version}.txt
        [ -e "${ref_err_file}" ] && break
    done
    if [ ! -e "${ref_err_file}" ]
    then
        echo ERROR: ref_err_file=${ref_err_file} not found
        exit 1
    fi
    # echo ref_err_file=${ref_err_file}

    # Remove previous test result files
    [ -e "${tst_out_file}" ] && rm -f ${tst_out_file}
    [ -e "${tst_err_file}" ] && rm -f ${tst_err_file}

    # Call tshark
    # display_filter_option="-R example_1.error_in_packet"
    $tshark  -n -r ${infile}  -T text -V  ${display_filter_option}  > ${tst_out_full_file}  2> ${tst_err_full_file}

    # Remove all details of upper level (TCP, IP, Ethernet, Frame)
    perl test_output_filter.pl  ${tst_out_full_file}  >  ${tst_out_file}

    # Remove some details
    perl test_error_filter.pl  ${tst_err_full_file}  >  ${tst_err_file}


    tst_chk_file  "err"  ${tst_err_file}  ${ref_err_file}  ${infile}
    tst_chk_file  "out"  ${tst_out_file}  ${ref_out_file}  ${infile}
}

# ***************************************************************************
# Compare 2 files (tst_file & ref_file) which should be identic
# ***************************************************************************
function tst_chk_file()
{
    chk_label=$1
    tst_file=$2
    ref_file=$3
    infile=$4
    
    set +e
    # Reference files come from windows, so ignore cr
    diff  --strip-trailing-cr  ${ref_file}  ${tst_file}  > /dev/null
    diff_ret=$?
    set -e
    if (( ${diff_ret} != 0 ))
    then
        # Files are different
        echo "NOT ok :  ${chk_label}  ${infile}"
        error_counter=$((${error_counter} + 1))

        if (( ${user_diff} == 1 ))
        then
            set +e
            diff  --strip-trailing-cr  ${ref_file}  ${tst_file}
            set -e
        fi
        if (( ${user_windiff} == 1 ))
        then
            "${windiff}"  ${ref_file}  ${tst_file}
        fi
    else
        echo "    ok :  ${chk_label}  ${infile}"
    fi
}



# ***************************************************************************
# Go
# ***************************************************************************
error_counter=0

if [ ! -z "$1" ]
then
    # Test specified sub-directories
    for dir in $*
    do
        [ ! -d ${dir} ] && echo "${dir} is not a directory" && continue
        tst_dir  ${dir}
    done
else
    # Test all proto_... sub-directories
    for dir in $(ls -d proto_*)
    do
        tst_dir  ${dir}
    done
fi

exit ${error_counter}

# tshark [ -a <capture autostop condition> ] ...
#        [ -b <capture ring buffer option>] ...
#        [ -B <capture buffer size (Win32 only)> ]  
#        [ -c <capture packet count> ] 
#        [ -C <configuration profile> ] 
#        [ -d <layer type>==<selector>,<decode-as protocol> ] 
#        [ -D ] 
#        [ -e <field> ] 
#        [ -E <field print option> ] 
#        [ -f <capture filter> ] 
#        [ -F <file format> ]
#        [ -h ] 
#        [ -i <capture interface>|- ] [ -l ] [ -L ] [ -n ] [ -N <name resolving flags> ] [ -o <preference setting> ] ... [ -p ] [ -q ] 
#        [ -r <infile> ] 
#        [ -R <read (display) filter> ] 
#        [ -s <capture snaplen> ] [ -S ] [ -t ad|a|r|d|e ] [ -T pdml|psml|ps|text|fields ] [ -v ] [ -V ] 
#        [ -w <outfile>|- ] 
#        [ -x ] [ -X <eXtension option>] [ -y <capture link type> ] [ -z <statistics> ] [ <capture filter> ]
