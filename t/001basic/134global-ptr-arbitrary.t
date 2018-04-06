#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/global-ptr-arbitrary.dt -o global-ptr-arbitrary `;
chomp for @res;
is_deeply(\@res, ['10', '100'], 'Got expected compilation messages');
@res = `./global-ptr-arbitrary`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['10', '100'],
    'Got correct results');

`rm global-ptr-arbitrary`;

1;
