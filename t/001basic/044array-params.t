#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/array-params.dt -o array-params  `;
is(@res, 0, 'No compilation errors');

@res = `./array-params`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'p int',
    '10',
    'p array-of 10 int',
    '20',
    'p array-of 9 int',
    '20',
], "Got expected results");

`rm array-params`;

1;
