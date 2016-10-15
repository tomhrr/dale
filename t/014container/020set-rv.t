#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/set-rv.dt -o set-rv `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./set-rv`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'2',
'3',
'4',
],
    'Got correct results');

`rm set-rv`;

1;
