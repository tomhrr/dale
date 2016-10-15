#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/pthread-basic.dt -o pthread-basic -l pthread `;
is(@res, 0, 'No compilation errors');

@res = `./pthread-basic`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is(@res, 100, 'Got expected results');

`rm pthread-basic`;

1;
