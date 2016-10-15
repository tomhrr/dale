#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/dispatch.dt -o dispatch`;
chomp for @res;
is_deeply(\@res, [qw(macro31
macro32
macro41
macro42
macro52
macro51
macro62
macro61)], 'Got correct compilation messages');

@res = `./dispatch`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(fn11 fn12 fn21 fn22)],
    'Got expected results');

`rm dispatch`;

1;
