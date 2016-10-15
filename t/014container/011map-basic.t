#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/map-basic.dt -o map-basic `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./map-basic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'Equal',
'1: 2',
'5: 6',
'7: 8',
'5: 6',
'7: 8',
'1: 2',
'Equal',
],
        'Got correct results');

`rm map-basic`;

1;
