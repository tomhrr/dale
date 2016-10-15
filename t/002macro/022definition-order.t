#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/definition-order.dt -o definition-order `;
chomp for @res;
is_deeply(\@res, ['other', 'other'], 'No compilation errors');
@res = `./definition-order`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['int', 'int'],
        'Got correct results');

`rm definition-order`;

1;
