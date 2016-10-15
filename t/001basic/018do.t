#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/do.dt -o do`;
is(@res, 0, 'No compilation errors');

@res = `./do`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 'Setting temp', 'Not just yet though', 'OK now!', 'Temp is 1' ],
                   'Got expected results');

`rm do`;

1;
