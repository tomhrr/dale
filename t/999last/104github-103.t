#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/fp-at-compile-time.dt -o fp-at-compile-time`;
is(@res, 0, 'No compilation errors');

@res = `./fp-at-compile-time`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, ['123 123'],
    'Got expected results');

`rm fp-at-compile-time`;

1;
