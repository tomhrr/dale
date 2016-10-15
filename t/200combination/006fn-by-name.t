#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/fn-by-args-name.dt -o fn-by-args-name `;
is(@res, 0, 'No compilation errors');

@res = `./fn-by-args-name`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(fn1 fn2 fn1)], 'Got expected results');

`rm fn-by-args-name`;

1;
