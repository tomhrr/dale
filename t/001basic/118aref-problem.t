#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/aref-problem.dt -o aref-problem`;
is(@res, 0, 'No compilation errors');

@res = `./aref-problem`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, ['2','2'],
    'Got expected results');

`rm aref-problem`;

1;
