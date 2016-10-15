#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-other-setf.dt -o sp-other-setf `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-other-setf`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '100 200',
    '1000 2000',
    '1000 2000',
],
        'Got correct results');

`rm sp-other-setf`;

1;
