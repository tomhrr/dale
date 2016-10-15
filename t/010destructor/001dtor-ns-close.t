#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtor-ns-close.dt -o dtor-ns-close `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./dtor-ns-close`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'100', 
'destroying 1.000000', 
'destroying 1.000000'
],
        'Got correct results');

`rm dtor-ns-close`;

1;
