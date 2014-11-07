#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/concept-tree.dt -o concept-tree `;
is(@res, 0, 'No compilation errors');

@res = `./concept-tree`;
is($?, 0, 'Program executed successfully');
chomp for @res;
is_deeply(\@res, [
    '2'
], 'Got expected results');

`rm concept-tree`;

1;
