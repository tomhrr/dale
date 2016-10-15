#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/nested-qq.dt -o nested-qq `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./nested-qq`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '6', '6.000000'
],
        'Got correct results');

`rm nested-qq`;

1;
