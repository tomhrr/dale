package Dale;

use warnings;
use strict;

sub is_x86_64
{
    my ($test_dir) = @_;

    my %config;

    open my $fh, '<', "./Config.h" or die $!;
    my @lines = <$fh>;
    close $fh;
    
    for (@lines) {
        my ($name, $value) = (/^#define (.+?)\s+(.+)$/);
        if (not $name) {
            die "Unable to parse config line: $_";
        }
        $value =~ s/^"//;
        $value =~ s/"$//;
        $config{$name} = $value;
    }
    
    my $is_x86_64 =
        (($config{'SYSTEM_PROCESSOR'} eq 'x86_64')
            or ($config{'SYSTEM_NAME'} eq 'Darwin'
                and $config{'SIZEOF_DATA_PTR'} == 8));

    return $is_x86_64;
}

1;
