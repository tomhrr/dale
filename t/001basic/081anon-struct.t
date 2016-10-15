#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/anon-struct.dt -o anon-struct `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./anon-struct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
          [ '10' ],
          'Got correct results');

`rm anon-struct`;

1;
