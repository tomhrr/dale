#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-make.dt -o sp-make `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-make`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '100' ],
        'Got correct results');

`rm sp-make`;

1;
