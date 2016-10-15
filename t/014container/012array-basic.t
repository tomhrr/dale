#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/array-basic.dt -o array-basic `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./array-basic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'10 10',
'0',
'First array is not empty',
'Second array is empty',
'1 1 1 1 1 1 1 1 1 1 ',
'9 8 7 6 5 4 3 2 1 0 ',
],
        'Got correct results');

`rm array-basic`;

1;
