#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/find-if-not.dt -o find-if-notd -l pthread `;
is(@res, 0, 'No compilation errors');

@res = `./find-if-notd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'Found 0',
'Found 2',
'Found 4',
'Found 6',
'Found 8',
'No more found',
],
    'Got expected results');

`rm find-if-notd`;

1;
