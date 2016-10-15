#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/init-array.dt -o init-array`;
is(@res, 0, 'No compilation errors');

@res = `./init-array`;
is($?, 0, 'Program executed successfully');

chomp for @res;
@res = map { s/0//g; $_ } @res;
is_deeply(\@res, ['123 456', '123 456'], 'Got expected results');

`rm init-array`;

1;
