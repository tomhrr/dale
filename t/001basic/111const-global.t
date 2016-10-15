#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/const-global.dt -o const-global`;
is(@res, 0, 'No compilation errors');

@res = `./const-global`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'100'
], 'Got expected results');

`rm const-global`;

1;
