#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/hex-literals.dt -o hex-literals `;
is(@res, 0, 'No compilation errors');

@res = `./hex-literals`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'1',
'10',
'11',
'15',
'240',
'65535',
'4294967295',
], 'Got expected results');

`rm hex-literals`;

1;
