#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/float-double.dt -o float-double `;
is(@res, 0, 'No compilation errors');

@res = `./float-double`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '10.000000',
    '20.000000',
    '20.000000 10.000000',
    '10.000000 20.000000',
], 'Got expected results');

`rm float-double`;

1;
