#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/typed-macro.dt -o typed-macro `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./typed-macro`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ '6', '6.000000', '8', '8.000000'
],
        'Got correct results');

`rm typed-macro`;

1;
