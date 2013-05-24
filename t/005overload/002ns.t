#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/over-ns.dt -o over-ns `;
is(@res, 0, 'No compilation errors');

@res = `./over-ns`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '300 400',
    '100 300',
    '200 400',
], 'Got expected results');

`rm over-ns`;

1;
