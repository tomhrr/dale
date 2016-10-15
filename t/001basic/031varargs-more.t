#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/varargs-more.dt -o varargs-more `;
is(@res, 0, 'No compilation errors');

@res = `./varargs-more`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'asdf', 'qwer', 10, 20,
    'zxcv', 'tyui', 30, 40 
], 'Got expected results');

`rm varargs-more`;

1;
