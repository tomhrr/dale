#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/used-ns-problem.dt -o used-ns-problem`;
is(@res, 0, 'No compilation errors');

@res = `./used-ns-problem`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(10)],
    'Got expected results');

`rm used-ns-problem`;

1;
