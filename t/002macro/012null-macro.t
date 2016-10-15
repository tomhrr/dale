#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/null-macro.dt -o null-macro   `;
is(@res, 0, 'No compilation errors');

@res = `./null-macro`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'pre null',
    'post null'
], 'Got expected results');

`rm null-macro`;

1;
