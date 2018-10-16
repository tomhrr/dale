#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/gh-205.dt -o gh-205`;
is(@res, 0, 'No compilation errors');

@res = `./gh-205`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is(@res, 1, 'Got one result');

`rm gh-205`;

1;
