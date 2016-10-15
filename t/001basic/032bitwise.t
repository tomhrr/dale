#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/bitwise.dt -o bitwise `;
is(@res, 0, 'No compilation errors');

@res = `./bitwise`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    1, 1, 0, 2, 4, 2, '4294967295'
], 'Got expected results');

`rm bitwise`;

1;
