#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-copy-self.dt -o sp-copy-self `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-copy-self`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '100', '100' ],
        'Got correct results');

`rm sp-copy-self`;

1;
