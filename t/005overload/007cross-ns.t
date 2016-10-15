#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/overload-cross-ns.dt -o overload-cross-ns `;
chomp for @res;
is_deeply(\@res, ['Called macro'],
     'Got expected compilation results');

@res = `./overload-cross-ns`;
is($?, 0, 'Program executed successfully');

chomp for @res;
my @final_res = map { s/\..*//; $_ } @res;
is_deeply(\@final_res, [
    'Called function',
    '6',
    '6'
], 'Got expected results');

`rm overload-cross-ns`;

1;
