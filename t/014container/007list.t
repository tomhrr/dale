#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/list.dt -o list `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./list`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'1 3 4 ',
'4 3 1 ',
'',
'',
'4 3 2 1 ',
'1 2 3 4 ',
'2 1 ',
'1 2 ',
'2 1 3 4 5 ',
'5 4 3 1 2 ',
'2 3 5 ',
'5 3 2 ',
'3 5 ',
'5 3 ',
'3 ',
'3 ',
'',
'',
'4 3 2 1 ',
'1 2 3 4 ',
'',
'',
],
        'Got correct results');

`rm list`;

1;
