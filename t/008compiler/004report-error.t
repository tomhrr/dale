#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/report-error.dt -o report-error    2>&1`;

ok($?, 'Program compilation failed');

chomp for @res;

is_deeply(\@res, [
    "$test_dir/t/src/report-error.dt:15:41: error: this node is not a type (see macro at 15:33)",
    "$test_dir/t/src/report-error.dt:15:33: error: macro expansion error (see previous)",
], 'Got expected error message');

1;
