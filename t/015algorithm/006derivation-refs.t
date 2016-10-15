#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/derivation-refs.dt -o derivation-refsd`;
is(@res, 0, 'No compilation errors');

@res = `./derivation-refsd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'm =  n? (0)',
'm != n? (1)',
'm <  n? (1)',
'm <= n? (1)',
'm >  n? (0)',
'm >= n? (0)',
],
    'Got expected results');

`rm derivation-refsd`;

1;
