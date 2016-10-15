#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/ptr-math.dt -o ptr-math`;
is(@res, 0, 'No compilation errors');

@res = `./ptr-math`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '0 1',
    '5 6',
    '3 4',
    'yes',
    'yes',
    'yes',
    'yes',
], 'Got expected results');

`rm ptr-math`;

1;
