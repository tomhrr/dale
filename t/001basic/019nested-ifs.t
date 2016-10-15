#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/nested-ifs.dt -o nested-ifs`;
is(@res, 0, 'No compilation errors');

@res = `./nested-ifs`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '1 2 3 4 5 6 7 8 9 10 11 13 15 17 19 21 24 27 30 33 36 39 42 Finished' ],
                   'Got expected results');

`rm nested-ifs`;

1;
