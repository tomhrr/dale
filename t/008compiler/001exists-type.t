#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/exists-type.dt -o exists-type   `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..2];

is_deeply(\@res, [
    'int The type exists',
    '(p (p (p int))) The type exists',
    'not-a-real-type The type does not exist',
], 'Got expected compilation results');

@res = `./exists-type`;
is($?, 0, 'Program executed successfully');
chomp for @res;

is_deeply(\@res, [ 
    'Type int exists',
    'Type ppp int exists',
    'Type not-a-real-type does not exist',
], 'Got expected results');

`rm exists-type`;

1;
