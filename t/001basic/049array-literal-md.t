#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/array-literal-md.dt -o array-literal-md  `;
is(@res, 0, 'No compilation errors');

@res = `./array-literal-md`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '1 2 3 4' ], 'Got expected results');

`rm array-literal-md`;

1;
