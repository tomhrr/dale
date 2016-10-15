#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/offsetof.dt -o offsetof `;
is(@res, 0, 'No compilation errors');

@res = `./offsetof`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is(@res, 1, 'Got one line of results');
like($res[0], qr/^0 \d+ \d+ \d+$/,
    'Got expected results ('.$res[0].')');

`rm offsetof`;

1;
