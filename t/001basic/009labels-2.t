#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/labels-2.dt -o labels-2`;
is(@res, 0, 'No compilation errors');

@res = `./labels-2`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'In loop, index is 0',
    'In inner loop, index is 0',
    'In inner loop, index is 1',
    'In inner loop, index is 2',
    'In loop, index is 1',
    'In inner loop, index is 0',
    'In inner loop, index is 1',
    'In inner loop, index is 2',
    'In loop, index is 2',
    'In inner loop, index is 0',
    'In inner loop, index is 1',
    'In inner loop, index is 2',
    'Finished loop',
], 'Got expected results');

`rm labels-2`;

1;
