#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/variant-multiple.dt -o variant-multiple`;
is(@res, 0, 'No compilation errors');

@res = `./variant-multiple`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'Left (expected)',
    'Right (expected)',
], 'Got expected results');

`rm variant-multiple`;

1;
