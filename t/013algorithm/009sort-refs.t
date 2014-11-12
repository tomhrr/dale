#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/sort-refs.dt -o sort-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./sort-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

# The periods indicate copy operations.  Whether all of these are
# currently required for the sort has not been verified, but it is
# still a big improvement over the non-PreferRefs version.
is_deeply(\@res,
[
'........Presort',
'...................Postsort',
],
    'Got expected results');

`rm sort-refsd`;

1;
