#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/vector.dt -o vector `;
is(@res, 0, 'No compilation errors');

@res = `./vector`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'Empty?: 1',
'1 2',
'3 4',
'1 2',
'3 4',
'1 2',
'Empty?: 0',
'Empty?: 1',
'for loop',
'1 2',
'3 4',
'1 2',
'3 4',
'3 4',
'iterator',
'1 2',
'3 4',
'1 2',
'3 4',
'3 4',
'riterator',
'1 2',
'1 2',
'1 2',
'1 2',
'1 2',
'pre copy',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'post copy',
'1 2',
'1 2',
'1 2',
'1 2',
'1 2',
'pre assign from two to one',
'post assign',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'100',
], 'Got expected results');

`rm vector`;

1;
