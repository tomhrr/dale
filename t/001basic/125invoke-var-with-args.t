#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/invoke-var-with-args.dt -o invoke-var-with-args`;
is(@res, 0, 'No compilation errors');

@res = `./invoke-var-with-args`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, ['called 1'],
    'Got expected results');

`rm invoke-var-with-args`;

1;
