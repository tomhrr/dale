#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/macros-varargs.dt -o macros-varargs  `;
is(@res, 0, 'No compilation errors');

@res = `./macros-varargs`;
is(($? >> 8), 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '15' ], 'Got expected results');

`rm macros-varargs`;

1;
