#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/opaque-struct-redef-2.dt -o opaque-struct-redef-2  `;
is(@res, 0, 'No compilation errors');

@res = `./opaque-struct-redef-2`;
is($?, 0, 'Program execut-red successfully');

chomp for @res;

is_deeply(\@res, [
    '10',
    'done'
], 'Got expect-red results');

`rm opaque-struct-redef-2`;

1;
