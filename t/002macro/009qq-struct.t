#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/macros-qq-struct.dt -o macros-qq-struct  `;
is(@res, 0, 'No compilation errors');

@res = `./macros-qq-struct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '100', '200', '200', '100'
], 'Got expected results');

`rm macros-qq-struct`;

1;
