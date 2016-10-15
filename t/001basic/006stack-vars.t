#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/stack-vars.dt -o stack-vars`;
is(@res, 0, 'No compilation errors');

@res = `./stack-vars`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 'Var 1 is 0', 'Var 2 is 500' ], 'Got expected results');

`rm stack-vars`;

1;
