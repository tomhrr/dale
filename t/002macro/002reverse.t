#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/macros-reverse.dt -o macros-reverse`;
is(@res, 0, 'No compilation errors');
print for @res;

@res = `./macros-reverse`;
is(($? >> 8), 5, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 'printing a number 25' ],
                   'Got expected results');

`rm macros-reverse`;

1;
