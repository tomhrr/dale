#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/const-parameters.dt -o const-parameters`;
is(@res, 0, 'No compilation errors');

@res = `./const-parameters`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'asdf'
], 'Got expected results');

`rm const-parameters`;

1;
