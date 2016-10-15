#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/varargs.dt -o varargs`;
is(@res, 0, 'No compilation errors');

@res = `./varargs`;
is(($? >> 8), 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '',
    'str1 ',
    'str1 str2 ',
    'str1 str2 str3 ',
    'str1 str2 str3 str4 ',
    'str1 str2 str3 str4 str5 ',
], 'Got expected results');

`rm varargs`;

1;
