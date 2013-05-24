#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/namespace-macs.dt -o namespace-macs  `;
is(@res, 0, 'No compilation errors');

@res = `./namespace-macs`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    'the identity macro',
    'not the identity macro'
], 'Got expected results');

`rm namespace-macs`;

1;
