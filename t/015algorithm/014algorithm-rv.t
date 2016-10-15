#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/algorithm-rv.dt -o algorithm-rv `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./algorithm-rv`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'Did not find pointer',
'Found pointer',
'Did not find lower bound for 4',
'Found lower bound for 3 (3)',
'Did not find upper bound for 3',
'Found upper bound for 2 (3)',
'Found 3',
'Did not find 4',
'Lower bound is 2',
'Upper bound is 3',
],
    'Got correct results');

`rm algorithm-rv`;

1;
