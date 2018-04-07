#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/array-ref-params.dt -o array-ref-params `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./array-ref-params`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['1234'],
    'Got correct results');

`rm array-ref-params`;

1;
