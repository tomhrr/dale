#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/arrays.dt -o arrays`;
is(@res, 0, 'No compilation errors');

@res = `./arrays`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '4 3 2 1 0', '5 6', '5 6' ], 'Got expected results');

`rm arrays`;

1;
