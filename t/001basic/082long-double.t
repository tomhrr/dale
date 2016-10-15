#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

# todo: if long-double is used, optimisation doesn't work
# (instcombine, at least).
my @res = `dalec -O0 $test_dir/t/src/long-double.dt -o long-double `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./long-double`;
is($?, 0, 'Program executed successfully');

chomp for @res;

like($res[0], qr/123.123/, 'Got correct results');

`rm long-double`;

1;
