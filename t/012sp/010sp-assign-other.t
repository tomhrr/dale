#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-assign-other.dt -o sp-assign-other `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-assign-other`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '1 1',
    '100 200',
    '100 100',
    '2 2',
],
        'Got correct results');

`rm sp-assign-other`;

1;
