#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/set-balanced-2.dt -o set-balanced-2 `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./set-balanced-2`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'complete'
],
    'Got correct results');

`rm set-balanced-2`;

1;
