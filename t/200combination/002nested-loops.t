#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/nested-loops.dt -o nested-loops `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./nested-loops`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'i: 0',
'j: 0',
'continue on j = 0',
'j: 1',
'k: 0',
'continue on k = 0',
'k: 1',
'0 1 1',
'k: 2',
'break on k = 2',
'j: 2',
'k: 0',
'continue on k = 0',
'k: 1',
'0 2 1',
'k: 2',
'break on k = 2',
'i: 1',
'break on i = 2',
],
        'Got correct results');

`rm nested-loops`;

1;
