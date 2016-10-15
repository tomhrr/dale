#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/signal.dt -o signal `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./signal`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['Caught sigint', 'Raised sigint',
                  'Caught sigint', 'Raised sigint'],
        'Got correct results');

`rm signal`;

1;
