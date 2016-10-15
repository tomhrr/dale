#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/rv-refs-move.dt -o rv-refs-move`;
is(@res, 0, 'No compilation errors');

@res = `./rv-refs-move`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'lvref 1',
'lvref 1',
'rvref 2',
'rvref 2',
'rvref 1',
'rvref 1',
], 'Got expected results');

`rm rv-refs-move`;

1;
