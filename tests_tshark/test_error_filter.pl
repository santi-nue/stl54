# 

# use strict;

$progname = $0;
$progname =~ s,.*/,,;     # use basename only
$progname =~ s/\.\w*$//;  # strip extension if any

my($in_filename) = shift;

if (! -e $in_filename)
{
    print "$in_filename does NOT exists.\n";
    return;
}

open (my $in_filename_HANDLE, $in_filename) or die "Fail to open file $in_filename\n";

# print "$in_filename\n";

while(<$in_filename_HANDLE>) {
#    print "$_";
    chomp;                  # remove \n
    my($file_line) = $_;    # get the current ligne
            
    my(@file_line_words) = split (' ', $file_line);

	if (($file_line_words[0] eq "proto_bad/empty.wsgd") || ($file_line_words[0] eq "proto_bad//empty.wsgd"))
	{
		# Change to windows path
		$file_line =~ s://:\\:g;
		$file_line =~ s:/:\\:g;
	}

	if (($file_line_words[0] eq "FATAL") && ($file_line_words[2] eq "PROTO_TYPE_DEFINITIONS"))
	{
		# Should be something like :
		# FATAL :  PROTO_TYPE_DEFINITIONS not found at T:\wireshark\dev\build_sources_4.0--win64\wireshark\plugins\epan\generic\T_generic_protocol_data_base.cpp[30] at T:\wireshark\dev\build_sources_4.0--win64\wireshark\plugins\epan\generic\byte_interpret_common_fatal.cpp line=51

		# Remove the paths
		# So identic (on 32/64 bits, on 4.0/3.6/..., on windows/linux)
		$file_line =~ s/[^ ]+T_generic_protocol_data_base.cpp/T_generic_protocol_data_base.cpp/g;
		$file_line =~ s/[^ ]+byte_interpret_common_fatal.cpp/byte_interpret_common_fatal.cpp/g;
	}
	
    print "$file_line\n";
}
