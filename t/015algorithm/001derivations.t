#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/derivations.dt -o derivationsd `;
is(@res, 0, 'No compilation errors');

@res = `./derivationsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'Not equal',
'Not equal',
'A is less than B',
'A is less than or equal to B',
'A is not more than B',
'A is not more than or equal to B',
], 'Got expected results');

`rm derivationsd`;

1;
