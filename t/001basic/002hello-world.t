#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/hello-world.dt -o hello-world`;
is(@res, 0, 'No compilation errors');

@res = `./hello-world`;
is($?, 0, 'Program executed successfully');
chomp for @res;

is_deeply(\@res, [ 'Hello world!' ], 'Got expected results');

`rm hello-world`;

1;
