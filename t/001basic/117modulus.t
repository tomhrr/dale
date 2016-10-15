#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/modulus.dt -o modulus`;
is(@res, 0, 'No compilation errors');

@res = `./modulus`;
is($?, 0, 'Program executed successfully');

chomp for @res;
$res[2] =~ s/\..*//;

is_deeply(\@res, ['2','2','2','2'],
    'Got expected results');

`rm modulus`;

1;
