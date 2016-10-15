#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/find.dt -o findd -l pthread `;
is(@res, 0, 'No compilation errors');
print for @res;

@res = `./findd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'Found 0',
'Found 2',
'Found 4',
'Found 6',
'Found 8',
'Did not find 10',
'Did not find 12',
'Did not find 14',
'Did not find 16',
'Did not find 18',
],

    'Got expected results');

`rm findd`;

1;
