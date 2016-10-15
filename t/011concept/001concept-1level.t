#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/concept-1level.dt -o concept-1level  `;
is(@res, 0, 'No compilation errors');

@res = `./concept-1level`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '6', '10'
], 'Got expected results');

`rm concept-1level`;

1;
