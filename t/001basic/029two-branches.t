#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/two-branches.dt -o two-branches`;
is(@res, 0, 'No compilation errors');

@res = `./two-branches`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'Begin',
    'In next block',
    'In end block',
    'i is 6',
], 'Got expected results');

`rm two-branches`;

1;
