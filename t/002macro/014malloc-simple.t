#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/malloc-simple.dt -o malloc-simple -l pthread `;
is(@res, 0, 'No compilation errors');

@res = `./malloc-simple`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(0 1 2 3 4 5 6 7 8 9)],
    'Got expected results');

`rm malloc-simple`;

1;