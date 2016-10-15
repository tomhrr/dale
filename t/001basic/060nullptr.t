#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/nullptr.dt -o nullptr `;
is(@res, 0, 'No compilation errors');

@res = `./nullptr`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'np is a null pointer',
'vp is a non-null pointer',
], 'Got expected results');

`rm nullptr`;

1;
