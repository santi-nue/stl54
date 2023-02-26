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
  

my($filter_not_empty_line_beginning_by_blank) = 0;

# print "$in_filename\n";


while(<$in_filename_HANDLE>) {
#    print "$_";
    chomp;                  # remove \n
    my($file_line) = $_;    # get the current ligne
            
    if ($file_line eq "")
    {
#        print "empty line\n";
        $filter_not_empty_line_beginning_by_blank = 0;
    }
    else
    {
        if (substr($file_line, 0, 1) ne ' ')
        {
#            print "not empty line, 1st word not empty\n";
            $filter_not_empty_line_beginning_by_blank = 0;
          
            my(@file_line_words) = split (' ', $file_line);

            if ($file_line_words[0] eq "Frame")
            {
#                print "not empty line, 1st word = Frame\n";
                $filter_not_empty_line_beginning_by_blank = 1;
            }
            elsif ($file_line_words[0] eq "Ethernet")
            {
#                print "not empty line, 1st word = Ethernet\n";
                $filter_not_empty_line_beginning_by_blank = 1;
            }
            elsif ($file_line_words[0] eq "Internet")
            {
#                print "not empty line, 1st word = Internet\n";
                $filter_not_empty_line_beginning_by_blank = 1;
            }
            elsif ($file_line_words[0] eq "Transmission")
            {
#                print "not empty line, 1st word = Transmission\n";
                $filter_not_empty_line_beginning_by_blank = 1;
            }
            elsif (($file_line_words[1] eq "Reassembled") ||
                   ($file_line_words[0] eq "[Reassembled"))
            {
#                print "not empty line, 1st word = Reassembled\n";
                $filter_not_empty_line_beginning_by_blank = 1;
            }
        }
        elsif ($filter_not_empty_line_beginning_by_blank == 1)
        {
#            print "not empty line, 1st word empty, ignored\n";
            next;
        }
    }
      
    print "$file_line\n";
}
