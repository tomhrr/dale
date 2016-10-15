#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/mr-fn.dt -o mr-fn  `;
is(@res, 0, 'No compilation errors');

@res = `./mr-fn`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'fn1: 9',
    'fn2: 8',
    'fn1: 7',
    'fn2: 6',
    'fn1: 5',
    'fn2: 4',
    'fn1: 3',
    'fn2: 2',
    'fn1: 1',
    'fn2: 0',
], 'Got expected results');

`rm mr-fn`;

1;
