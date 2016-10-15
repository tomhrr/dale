#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/exists-variable.dt -o exists-variable   `;

is($?, 0, 'Program compiled successfully');

chomp for @res;
@res = @res[0..4];

is_deeply(\@res, [
    'variable exists!',
    'variable does not exist',
    'variable exists!',
    'variable exists!',
    'variable does not exist',
], 'Got expected compilation results');

`rm exists-variable`;

1;
