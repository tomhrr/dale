#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/mfor-sum.dt -o mfor-sum `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./mfor-sum`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'adding 0 to sum',
    'adding 1 to sum',
    'adding 2 to sum',
    'adding 3 to sum',
    'adding 4 to sum',
    'now sum is 10', ],
        'Got correct results');

`rm mfor-sum`;

1;
