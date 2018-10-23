#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/gh-206.dt -o gh-206`;
is(@res, 0, 'No compilation errors');

@res = `./gh-206`;

chomp for @res;
is(@res, 2, 'Got two result lines');
like($res[0], qr/^ 00 00 00 00 .. .. .. .. 01 00 00 00 00 00 00 00$/,
    'Got expected result for line 1 (value begins at 8th byte)');
like($res[1], qr/^ 01 00 00 00 .. .. .. .. 00 00 80 3f 00 00 80 3f$/,
    'Got expected result for line 2 (value begins at 8th byte)');

`rm gh-206`;

1;
