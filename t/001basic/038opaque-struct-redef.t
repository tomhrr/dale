#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/opaque-struct-redef.dt -o opaque-struct-redef`;
is(@res, 0, 'No compilation errors');

@res = `./opaque-struct-redef`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '10',
    'All good'
], 'Got expected results');

`rm opaque-struct-redef`;

1;
