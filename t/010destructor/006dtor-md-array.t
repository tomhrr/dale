#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtor-md-array.dt -o dtor-md-array `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./dtor-md-array`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'destroying 4.000000',
'destroying 2.000000',
'destroying 0.000000',
'destroying 2.000000',
'destroying 1.000000',
'destroying 0.000000',
'destroying 0.000000',
'destroying 0.000000',
'destroying 0.000000',
],
        'Got correct results');

`rm dtor-md-array`;

1;
