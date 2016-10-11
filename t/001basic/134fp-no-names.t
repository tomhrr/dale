#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -I. -lm $test_dir/t/src/fp-no-names.dt -o fp-no-names`;
is(@res, 0, 'No compilation errors');

@res = `./fp-no-names`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [qw(3)],
    'Got expected results');

`rm fp-no-names`;

1;
