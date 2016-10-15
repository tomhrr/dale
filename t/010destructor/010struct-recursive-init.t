#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/struct-recursive-init.dt -o struct-recursive-init `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./struct-recursive-init`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ qw(0 0 1 1 2 2 3 3) ], 'Got correct results');

`rm struct-recursive-init`;

1;
