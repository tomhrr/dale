#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/enum-cmp.dt -o enum-cmp  `;
is(@res, 0, 'No compilation errors');

@res = `./enum-cmp`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ qw(English Spanish French) ], 'Got expected results');

`rm enum-cmp`;

1;
