#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/negative-array-indices.dt -o negative-array-indices`;
is(@res, 0, 'No compilation errors');

@res = `./negative-array-indices`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(200)],
    'Got expected results');

`rm negative-array-indices`;

1;
