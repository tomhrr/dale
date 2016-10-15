#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/zero-len-arrays.dt -o zero-len-arrays `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./zero-len-arrays`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'1',
'2',
'3',
'4',
'5',
],
        'Got correct results');

`rm zero-len-arrays`;

1;
