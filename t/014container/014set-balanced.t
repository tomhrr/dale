#!/usr/bin/env perl

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
'cbal',
'bal',
'cbal',
'bal',
'cbal',
'bal',
'cbal',
'bal',
'cbal',
'second set',
'bal',
'cbal',
'bal',
'cbal',
'third set',
'bal',
'cbal',
'bal',
'cbal',
],
    'Got correct results');

`rm set-balanced`;

1;
