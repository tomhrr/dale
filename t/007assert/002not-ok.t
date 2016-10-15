#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/assert-not-ok.dt -o assert-not-ok `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./assert-not-ok 2>&1`;
isnt($?, 0, 'Program did not execute successfully');

chomp for @res;

is_deeply(\@res,
    [ "$test_dir/t/src/assert-not-ok.dt:6:17: Assertion '(= 0 1)' failed." ],
    'Got correct results');

`rm assert-not-ok`;

1;
