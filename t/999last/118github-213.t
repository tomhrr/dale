#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/void-ptr-arithmetic.dt -o void-ptr-arithmetic `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./void-ptr-arithmetic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ["Pre-add", "Pre-subtract", "Finished"],
    'Got correct results');

`rm void-ptr-arithmetic`;

1;
