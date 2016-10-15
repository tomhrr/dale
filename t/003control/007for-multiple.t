#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/for-multiple.dt -o for-multiple  `;
is(@res, 0, 'No compilation errors');

@res = `./for-multiple`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'0 0',
'1 1',
'2 2',
'3 3',
'4 4',
'5 5',
'6 6',
'7 7',
'8 8',
'9 9',
], 'Got expected results');

`rm for-multiple`;

1;
