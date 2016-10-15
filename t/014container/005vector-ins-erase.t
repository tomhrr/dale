#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/vector-ins-erase.dt -o vector-ins-erase `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./vector-ins-erase`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'Three elements',
    '300',
    '200',
    '100',
    'First and third element',
    '300',
    '100',
    'Third element',
    '100',
    'No elements',
],
        'Got correct results');

`rm vector-ins-erase`;

1;
