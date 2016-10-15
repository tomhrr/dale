#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/import-within-fn.dt -o import-within-fn`;
is(@res, 0, 'No compilation errors');

@res = `./import-within-fn`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is(@res, 1, 'Got expected result count');
like($res[0], qr/^\d+$/, 'Got random number in results');

`rm import-within-fn`;

1;
