#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/array-of.dt -o array-of`;
is(@res, 0, 'No compilation errors');

@res = `./array-of`;
is($?, 0, 'Program executed successfully');

chomp for @res;
@res = map { s/0//g; $_ } @res;
is_deeply(\@res, ['.1 .2'], 'Got expected results');

`rm array-of`;

1;
