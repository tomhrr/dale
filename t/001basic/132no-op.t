#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/no-op.dt -o no-op`;
is(@res, 0, 'No compilation errors');

@res = `./no-op`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, ['1'], 'Got expected results');

`rm no-op`;

1;
