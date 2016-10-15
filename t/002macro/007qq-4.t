#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/macros-qq-4.dt -o macros-qq-4  `;
is(@res, 0, 'No compilation errors');

@res = `./macros-qq-4`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    "Wrap function start",
    "0",
    "1",
    "2",
    "4",
    "Wrap function end"
], 'Got expected results');

`rm macros-qq-4`;

1;
