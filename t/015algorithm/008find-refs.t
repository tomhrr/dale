#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/find-refs.dt -o find-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./find-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

# The periods indicate copy operations, which shouldn't occur during
# the find calls, due to the use of PreferRefs.
is_deeply(\@res,
[
'....Populated list',
'Called find',
'1 3 4',
'2 3 4',
'Called find-if',
'1 3 4',
'2 3 4',
'Called find-if-not',
'1 2 3',
'1 2 4',
'1 3 4',
'2 3 4',
],
    'Got expected results');

`rm find-refsd`;

1;
