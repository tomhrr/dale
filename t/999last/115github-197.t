#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 1;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -c $test_dir/t/src/lfc-problem.dt -o lfc-problem`;
is(@res, 0, 'No compilation errors');

`rm lfc-problem`;

1;
