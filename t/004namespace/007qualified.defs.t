#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/namespace-qualified-defs.dt -o namespace-qualified-defs `;
is(@res, 0, 'No compilation errors');

@res = `./namespace-qualified-defs`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'100',
'101',
'1 2',
'macro test',
'2',
], 'Got expected results');

`rm namespace-qualified-defs`;

1;
