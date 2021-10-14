package Dale;

use warnings;
use strict;

sub _get_config
{
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

    return \%config;
}

sub is_x86_64
{
    my ($test_dir) = @_;

    my %config = %{_get_config()};

     my $is_x86_64 =
         (($config{'SYSTEM_PROCESSOR'} eq 'x86_64')
            or (($config{'SYSTEM_NAME'} eq 'Darwin')
                and ($config{'SIZEOF_DATA_PTR'} == 8)
                and ($config{'SYSTEM_PROCESSOR'} ne 'arm64')));

    return $is_x86_64;
}

sub is_arm64_apple
{
    my %config = %{_get_config()};

     my $is_arm64_apple =
         (($config{'SYSTEM_PROCESSOR'} eq 'arm64')
            and (($config{'SYSTEM_NAME'} eq 'Darwin')));

    return $is_arm64_apple;
}

1;
