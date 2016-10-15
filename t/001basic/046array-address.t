#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

# Not checking output here, because if the program compiles and
# executes then what you're testing works.

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/array-address.dt -o array-address  `;
is(@res, 0, 'No compilation errors');

@res = `./array-address`;
is($?, 0, 'Program executed successfully');

chomp for @res;

`rm array-address`;

1;
