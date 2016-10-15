#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-self-ref.dt -o sp-self-ref `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-self-ref`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
    '1 1',
    '100 200',
    '100 200',
    '1 1',
],
        'Got correct results');

`rm sp-self-ref`;

1;
