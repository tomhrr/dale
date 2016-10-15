#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/input-type-basic.dt -o input-type-basic `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..4];

is_deeply(\@res, [
    'size',
    '(p (const char))',
    '(p void)',
    '(p void)',
    'size'
], 'Got expected compilation results');

@res = `./input-type-basic`;
is($?, 0, 'Program executed successfully');

`rm input-type-basic`;

1;
