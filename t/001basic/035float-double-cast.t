#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/float-double-cast.dt -o float-double-cast `;
is(@res, 0, 'No compilation errors');

@res = `./float-double-cast`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    '20.000000',
    '20.000000',
    '0.000000',
    '100.000000',
], 'Got expected results');

`rm float-double-cast`;

1;
