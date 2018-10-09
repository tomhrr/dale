#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/gh-199.dt -o gh-199`;
is(@res, 0, 'No compilation errors');

@res = `./gh-199`;

chomp for @res;
is_deeply(\@res, ['1'],
    'Got expected results');

`rm gh-199`;

1;
