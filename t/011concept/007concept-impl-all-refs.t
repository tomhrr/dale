#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/concept-impl-all-refs.dt -o concept-impl-all-refs `;
is(@res, 0, 'No compilation errors');

@res = `./concept-impl-all-refs`;
is($?, 0, 'Program executed successfully');
chomp for @res;
is_deeply(\@res, [
    '5', '7', '3'
], 'Got expected results');

`rm concept-impl-all-refs`;

1;
