#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/refs-pass.dt -o refs-pass`;
is(@res, 0, 'No compilation errors');

@res = `./refs-pass`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'5 8'
], 'Got expected results');

`rm refs-pass`;

1;
