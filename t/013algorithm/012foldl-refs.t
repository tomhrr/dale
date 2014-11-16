#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/foldl-refs.dt -o foldl-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./foldl-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'....prefold',
'.postfold',
'6 12 18',
],
    'Got expected results');

`rm foldl-refsd`;

1;
