#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sizeof-array.dt -o sizeof-array  `;
is(@res, 0, 'No compilation errors');

@res = `./sizeof-array`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '4 40'
], "Got expected results (assumes that sizeof(int) == 4)");

`rm sizeof-array`;

1;
