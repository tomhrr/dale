#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/assert-disabled.dt -o assert-disabled `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./assert-disabled`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
          [ 'hello' ],
          'Got correct results');

`rm assert-disabled`;

1;
