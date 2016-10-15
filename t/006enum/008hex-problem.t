#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/enum-hex-problem.dt -o enum-hex-problemd `;
is(@res, 0, 'No compilation errors');

@res = `./enum-hex-problemd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [1, 2],
    'Got expected results');

`rm enum-hex-problemd`;

1;
