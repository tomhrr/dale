#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/if-one-branch.dt -o if-one-branch`;
is(@res, 0, 'No compilation errors');

@res = `./if-one-branch`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['done'],
    'Got expected results');

`rm if-one-branch`;

1;
