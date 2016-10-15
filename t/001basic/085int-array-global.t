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
    $is_x86_64 ? "int-array-global-x86-64.dt"
               : "int-array-global.dt";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/$filename -o int-array-global `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./int-array-global`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ (qw(11 11 12 13)) x ($is_x86_64 ? 10 : 8) ],
        'Got correct results');

`rm int-array-global`;

1;
