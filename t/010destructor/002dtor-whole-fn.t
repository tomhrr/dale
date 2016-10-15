#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtor-whole-fn.dt -o dtor-whole-fn `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./dtor-whole-fn`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'100', 
'destroying 1.000000', 
'destroying 1.000000', 
'destroying 9.000000', 
'destroying 8.000000', 
'destroying 7.000000'
],
        'Got correct results');

`rm dtor-whole-fn`;

1;
