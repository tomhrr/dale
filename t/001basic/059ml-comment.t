#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/ml-comment.dt -o ml-commentd `;
is(@res, 0, 'No compilation errors');

@res = `./ml-commentd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [0],
    'Got expected results');

`rm ml-commentd`;

1;
