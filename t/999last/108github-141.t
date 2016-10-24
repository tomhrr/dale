#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/empty-lists.dt -o empty-lists`;
chomp for @res;
is_deeply(\@res, ['()'], 'Got correct compilation messages');

@res = `./empty-lists`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(1)],
    'Got expected results');

`rm empty-lists`;

1;
