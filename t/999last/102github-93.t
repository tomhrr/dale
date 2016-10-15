#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/operator-macro-problems.dt -o operator-macro-problems`;
is(@res, 0, 'No compilation errors');

@res = `./operator-macro-problems`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(0 1 2 3 4 5 1 2 3 4 5 6 6)],
    'Got expected results');

`rm operator-macro-problems`;

1;
