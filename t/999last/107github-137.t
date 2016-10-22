#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/first-element-list.dt -o first-element-list`;
is(@res, 0, 'No compilation errors');

@res = `./first-element-list`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(3 7)],
    'Got expected results');

`rm first-element-list`;

1;
