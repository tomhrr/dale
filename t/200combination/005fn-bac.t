#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/fn-by-args-count.dt -o fn-by-args-count  `;
is(@res, 0, 'No compilation errors');

@res = `./fn-by-args-count`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(2 1)], 'Got expected results');

`rm fn-by-args-count`;

1;
