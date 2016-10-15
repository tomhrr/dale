#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/bitfields.dt -o bitfields `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./bitfields`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(0 1 0 0 1 2 3 0 0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 0)],
        'Got correct results');

`rm bitfields`;

1;
