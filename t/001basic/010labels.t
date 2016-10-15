#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/labels.dt -o labels`;
is(@res, 0, 'No compilation errors');

@res = `./labels`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'In loop, index is 0',
    'In loop, index is 1',
    'In loop, index is 2',
    'In loop, index is 3',
    'In loop, index is 4',
    'In loop, index is 5',
    'In loop, index is 6',
    'In loop, index is 7',
    'In loop, index is 8',
    'In loop, index is 9',
    'Finished loop'
], 'Got expected results');

`rm labels`;

1;
