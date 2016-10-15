#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/mqos-errors.dt -o mqos-errors    2>&1`;

ok($?, 'Program failed to compile');

chomp for @res;

is_deeply(\@res, [
    "$test_dir/t/src/mqos-errors.dt:88:11: error: type int3 has no corresponding list type (see macro at 88:5)",
    "$test_dir/t/src/mqos-errors.dt:88:5: error: macro expansion error (see previous)",
    "$test_dir/t/src/mqos-errors.dt:89:11: error: type int4 has no corresponding list type (see macro at 89:5)",
    "$test_dir/t/src/mqos-errors.dt:89:5: error: macro expansion error (see previous)",
], 'Got expected compilation results');

1;
