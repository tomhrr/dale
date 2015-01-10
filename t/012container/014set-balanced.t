#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/set-balanced.dt -o set-balanced `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./set-balanced`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'first set',
'bal',
'bal',
'bal',
'bal',
'bal',
'second set',
'bal',
'bal',
'third set',
'bal',
'bal',
],
    'Got correct results');

`rm set-balanced`;

1;
