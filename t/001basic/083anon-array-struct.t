#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/anon-array-struct.dt -o anon-array-struct `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./anon-array-struct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

like($res[0], qr/10 c 100/, 'Got correct results');

`rm anon-array-struct`;

1;
