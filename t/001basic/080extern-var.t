#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -I$test_dir $test_dir/t/src/extern-var.dt -o extern-var `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./extern-var`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
          [ '100' ],
          'Got correct results');

`rm extern-var`;

1;
