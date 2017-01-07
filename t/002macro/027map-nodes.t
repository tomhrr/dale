#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/map-nodes.dt -o map-nodes    2>&1`;

is($?, 0, 'Program compiled successfully');

chomp for @res;

is_deeply(\@res, [
    '(int)',
    '(int float)'
], 'Got expected compilation results');

system("rm map-nodes");

1;
