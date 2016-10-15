#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/array-rv.dt -o array-rv `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./array-rv`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'3',
'2',
'1',
'0',
'0',
'0',
'0',
'0',
'0',
'0',
],
    'Got correct results');

`rm array-rv`;

1;
