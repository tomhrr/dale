#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/math-helpers.dt -o math-helpers `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./math-helpers`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(11 5 24 2 2 0 1 0 0 1 1)],
        'Got correct results');

`rm math-helpers`;

1;
