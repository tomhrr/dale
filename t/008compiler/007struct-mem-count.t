#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/struct-member-count.dt -o struct-member-count `;
is(@res, 0, 'No compilation errors');

@res = `./struct-member-count`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 2,3,0 ], 'Got expected results');

`rm struct-member-count`;

1;
