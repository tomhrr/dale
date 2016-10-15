#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/macro-arg-collision.dt -o macro-arg-collision  `;
is(@res, 0, 'No compilation errors');

@res = `./macro-arg-collision`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ ], 'Got expected results');

`rm macro-arg-collision`;

1;
