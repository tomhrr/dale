#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/struct-literal-array.dt -o struct-literal-array`;
is(@res, 0, 'No compilation errors');

@res = `./struct-literal-array`;
is($?, 0, 'Program executed successfully');

chomp for @res;
@res = map { s/0//g; $_ } @res;
is_deeply(\@res, ['1 2'], 'Got expected results');

`rm struct-literal-array`;

1;
