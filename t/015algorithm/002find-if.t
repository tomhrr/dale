#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/find-if.dt -o find-ifd -l pthread `;
is(@res, 0, 'No compilation errors');

@res = `./find-ifd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'Found 1',
'Found 3',
'Found 5',
'Found 7',
'Found 9',
],
    'Got expected results');

`rm find-ifd`;

1;
