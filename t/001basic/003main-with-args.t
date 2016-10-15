#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/main-with-args.dt -o main-with-args`;
is(@res, 0, 'No compilation errors');

@res = `./main-with-args`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is(@res, 0, 'Got expected results');

if (@res) {
    print Dumper(\@res);
}

`rm main-with-args`;

1;
