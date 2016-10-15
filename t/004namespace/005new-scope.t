#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/new-scope.dt -o new-scope  `;
is(@res, 0, 'No compilation errors');

@res = `./new-scope`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
qw(1 0 2 3 3 2 0 0 0)
], 'Got expected results');

`rm new-scope`;

1;
