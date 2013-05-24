#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/if-proper.dt -o if-proper `;
is(@res, 0, 'No compilation errors');

@res = `./if-proper`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '2', '4', '6', '8', '100'
], 'Got expected results');

`rm if-proper`;

1;
