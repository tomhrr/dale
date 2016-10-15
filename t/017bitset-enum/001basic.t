#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/bitset-enum-basic.dt -o bitset-enum-basic`;
is(@res, 0, 'No compilation errors');

@res = `./bitset-enum-basic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '0', '1', '2', '4', '8'
], 'Got expected results');

`rm bitset-enum-basic`;

1;
