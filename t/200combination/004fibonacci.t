#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/fibonacci.dt -o fibonacci`;
is(@res, 0, 'No compilation errors');

@res = `./fibonacci`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
 '1 1 2 3 5 8 13 21 34 55 Finished'
], 'Got expected results');

`rm fibonacci`;

1;
