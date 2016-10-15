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
    $is_x86_64 ? "gstruct-x86-64.dt"
               : "gstruct.dt";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/$filename -o gstruct `;
is(@res, 0, 'No compilation errors');

@res = `./gstruct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

if ($is_x86_64) {
    is_deeply(\@res, [
        '7f',
        'ff',
        '7fff',
        'ffff',
        '7fffffff',
        'ffffffff',
        '7fffffffffffffff',
        'ffffffffffffffff',
        '7fffffffffffffffffffffffffffffff',
        'ffffffffffffffffffffffffffffffff',
    ], 'Got expected results');
} else {
    is_deeply(\@res, [
        '7f',
        'ff',
        '7fff',
        'ffff',
        '7fffffff',
        'ffffffff',
        '7fffffffffffffff',
        'ffffffffffffffff',
    ], 'Got expected results');
}

`rm gstruct`;

1;
