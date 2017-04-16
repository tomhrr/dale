#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/include-test.dt -I$test_dir/t/src -o include-test`;
is(@res, 0, 'No compilation errors');

@res = `./include-test`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [1000], 'Got expected results');

`rm include-test`;

1;
