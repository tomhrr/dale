#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/cf-while-continue.dt -o cf-while-continue  `;
is(@res, 0, 'No compilation errors');

@res = `./cf-while-continue`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'n is 10',
    'n is 9',
    'n is 7',
    'n is 6',
    'n is 5',
    'n is 4',
    'n is 3',
    'n is 2',
    'n is 1',
    'Finished',
], 'Got expected results');

`rm cf-while-continue`;

1;
