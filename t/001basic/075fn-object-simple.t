#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/fn-object-simple.dt -o fn-object-simple `;
is(@res, 0, 'No compilation errors');

@res = `./fn-object-simple`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
    '7'
], 'Got expected results');

`rm fn-object-simple`;

1;
