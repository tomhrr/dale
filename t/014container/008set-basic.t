#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/set-basic.dt -o set-basic `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./set-basic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'Set is empty',
'',
'Inserted element',
'Set is not empty',
'3 ',
'Should be 1..5',
'1 2 3 4 5 ',
'Should be 1, 2, 4, 5',
'1 2 4 5 ',
'Should be 2, 4, 5',
'2 4 5 ',
'Should be 4, 5',
'4 5 ',
'Should be 5',
'5 ',
'Set is not empty',
'Should now be empty',
'',
'Set is empty',
'Set is not empty',
'Should be 1..5',
'1 2 3 4 5 ',
],
        'Got correct results');

`rm set-basic`;

1;
