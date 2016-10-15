#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/exists-fn.dt -o exists-fn   `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..3];

is_deeply(\@res, [
    '(bool < int int) The function exists',
    '(2 3 4) The function does not exist',
    '(void myfn) The function exists',
    '(void mynextfn) The function exists',
], 'Got expected compilation results');

@res = `./exists-fn`;
is($?, 0, 'Program executed successfully');
chomp for @res;

is_deeply(\@res, [ 
    '< int int -> bool exists',
    'random stuff function does not exist',
    'void-void function exists',
    'void-void overloaded function exists'
], 'Got expected results');

`rm exists-fn`;

1;
