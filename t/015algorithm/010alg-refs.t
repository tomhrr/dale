#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/alg-refs.dt -o alg-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./alg-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'........Pre for-each',
'1 2 3',
'1 2 4',
'1 3 4',
'2 3 4',
'Post for-each',
'1 2 4',
'Post lower-bound',
'1 3 4',
'Post upper-bound',
'1 3 4',
'Post binary-search',
'.2 3 4',
'Post max',
'.1 2 3',
'Post min',
'....Post copy',
'....Post copy',
'1 2 3',
'1 2 4',
'1 3 4',
'2 3 4',
'..Post assign',
'1 2 3',
'1 2 4',
'......Post regen',
'Eq: 1',
'Lt: 1',
],
    'Got expected results');

`rm alg-refsd`;

1;
