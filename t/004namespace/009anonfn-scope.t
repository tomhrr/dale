#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 1;

my $res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/anonfn-scope.dt -o anonfn-scope`;
is($res, "*Struct* exists\n*Struct* exists\n", "Got expected results");

`rm anonfn-scope`;

1;
