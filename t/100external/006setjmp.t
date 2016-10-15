#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/setjmp.dt -o setjmp `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./setjmp`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(setjmp longjmp)],
        'Got correct results');

`rm setjmp`;

1;
