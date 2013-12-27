#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/retval-unneeded.dt -o retval-unneeded`;
is(@res, 0, 'No compilation errors');

@res = `./retval-unneeded`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'3 3'
], 'Got expected results');

`rm retval-unneeded`;

1;
