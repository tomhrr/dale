#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/misc-globals.dt -o misc-globals `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./misc-globals`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'asdf',
'zxcv',
'qwer',
'ghjk',
'tyui',
'(zxcv) (2) (4)',
'(zxcv) (2) (4)',
],
        'Got correct results');

`rm misc-globals`;

1;
