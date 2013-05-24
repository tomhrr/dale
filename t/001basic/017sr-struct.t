#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sr-struct.dt -o sr-struct`;
is(@res, 0, 'No compilation errors');

@res = `./sr-struct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ qw(10 20 30 10 20 30 40 50) ], 'Got expected results');

`rm sr-struct`;

1;
