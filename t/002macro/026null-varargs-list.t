#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/null-varargs-list.dt -o null-varargs-list `;
chomp for @res;
is_deeply(\@res, [], 'No compilation errors');
@res = `./null-varargs-list`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(begin end begin 1 end begin end)],
        'Got correct results');

`rm null-varargs-list`;

1;
