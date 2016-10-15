#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/bitset-enum-values.dt -o bitset-enum-values`;
is(@res, 0, 'No compilation errors');

@res = `./bitset-enum-values`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '2', '4', '16', '32'
], 'Got expected results');

`rm bitset-enum-values`;

1;
