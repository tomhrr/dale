#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/over-simple.dt -o over-simple `;
is(@res, 0, 'No compilation errors');

@res = `./over-simple`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'fn for void',
    'fn for s1',
    'fn for s2',
    'fn for s3',
    'fn for int',
    'fn for char',
], 'Got expected results');

`rm over-simple`;

1;
