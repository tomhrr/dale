#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/foldl.dt -o foldld`;
is(@res, 0, 'No compilation errors');

@res = `./foldld`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'10',
],
    'Got expected results');

`rm foldld`;

1;
