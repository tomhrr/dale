#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -s bc $test_dir/t/src/link-bc-2.dt`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/link-bc-1.dt -b $test_dir/t/src/link-bc-2.dt.bc`;
is_deeply(\@res, [], 'No compilation errors');

`./a.out`;
is($? >> 8, 100, 'Program executed successfully');

`rm $test_dir/t/src/link-bc-2.dt.bc`;

1;
