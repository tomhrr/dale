#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/quote.dt -o quote `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./quote`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 'string: Yo dude' ],
        'Got correct results');

`rm quote`;

1;
