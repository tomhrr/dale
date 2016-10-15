#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-nested.dt -o sp-nested `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-nested`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'Begin scope 1',
'Begin scope 2',
'Begin scope 3',
'End scope 3',
'End scope 2',
'End scope 1',
],
        'Got correct results');

`rm sp-nested`;

1;
