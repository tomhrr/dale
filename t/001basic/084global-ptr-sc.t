#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/global-ptr-sc.dt -o global-ptr-sc `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./global-ptr-sc`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'5 5',
'100 100'
],
        'Got correct results');

`rm global-ptr-sc`;

1;
