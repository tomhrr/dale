#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtor-struct.dt -o dtor-struct `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./dtor-struct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'destroying mys1 4',
'destroying mys1 3',
'destroying mys1 2',
'destroying mys1 1',
'destroying float',
],
        'Got correct results');

`rm dtor-struct`;

1;
