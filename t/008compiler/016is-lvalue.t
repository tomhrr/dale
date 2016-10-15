#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 1;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/is-lvalue.dt -o is-lvalue  `;
chomp for @res;
is_deeply(\@res, [qw(0 1 1 1 1 1 0)], 'Got expected results');
`rm is-lvalue`;

1;
