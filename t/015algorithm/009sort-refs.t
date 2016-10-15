#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/sort-refs.dt -o sort-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./sort-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

# The periods indicate copy operations.  For the sort, there are
# fifteen copies, representing five swaps.
is_deeply(\@res,
[
'....Presort',
'...............Postsort',
'1 2 3',
'1 2 4',
'1 3 4',
'2 3 4',
],
    'Got expected results');

`rm sort-refsd`;

1;
