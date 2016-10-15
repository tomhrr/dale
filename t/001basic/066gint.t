#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir;
BEGIN { $test_dir = $ENV{"DALE_TEST_DIR"} || "." };
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

use lib "$test_dir/t/lib";
use Dale;

my $is_x86_64 = Dale::is_x86_64($test_dir);

my $filename = 
    $is_x86_64 ? "gint-x86-64.dt" 
               : "gint.dt";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/$filename -o gint `;
is(@res, 0, 'No compilation errors');

@res = `./gint`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [ 
    '7f',
    'ff',
    '7fff',
    'ffff',
    '7fffffff',
    'ffffffff',
    '7fffffffffffffff',
    'ffffffffffffffff',
    ($is_x86_64)
        ? ('7fffffffffffffffffffffffffffffff',
           'ffffffffffffffffffffffffffffffff')
        : ()
], 'Got expected results');

`rm gint`;

1;
