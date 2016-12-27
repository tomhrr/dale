#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/macros-qq-successive-uql.dt -o macros-qq-successive-uql `;
chomp for @res;
is_deeply(\@res, [], 'No compilation errors');
@res = `./macros-qq-successive-uql`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['1 2 3 4 5 6 7'],
        'Got correct results');

`rm macros-qq-successive-uql`;

1;
