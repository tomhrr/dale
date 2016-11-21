#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 1;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/incorrect-cto-error.dt -o incorrect-cto-error`;
is(@res, 0, 'No compilation errors');

`rm incorrect-cto-error`;

1;
