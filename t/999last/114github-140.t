#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/unquote-arbitrary.dt -o unquote-arbitrary`;
is(@res, 0, 'No compilation errors');

@res = `./unquote-arbitrary`;

chomp for @res;
is_deeply(\@res, ['1', '10'],
    'Got expected results');

`rm unquote-arbitrary`;

1;
