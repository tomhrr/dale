#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/vector-clear.dt -o vector-clear `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./vector-clear`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'Three elements',
    '300',
    '200',
    '100',
    'No elements',
],
        'Got correct results');

`rm vector-clear`;

1;
