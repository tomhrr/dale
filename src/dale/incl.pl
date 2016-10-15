#!/usr/bin/env perl

use warnings;
use strict;

my @paths = map { chomp; $_ } `find . -type f`;
for my $path (@paths) {
    my $includes = `cat $path | grep '^#include' | wc -l`;
    chomp $includes;
    if ($includes > 10) {
        print "$path: $includes\n";
    }
}
