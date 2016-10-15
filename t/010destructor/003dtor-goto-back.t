#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtor-goto-back.dt -o dtor-goto-back `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./dtor-goto-back`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'destroying 0.000000',
    'destroying 1.000000',
    'destroying 2.000000',
    'destroying 3.000000',
    'destroying 4.000000',
    'destroying 5.000000',
    'destroying 6.000000',
    'destroying 7.000000',
    'destroying 8.000000',
    'destroying 9.000000',
    'destroying 10.000000',
], 'Got correct results');

`rm dtor-goto-back`;

1;
