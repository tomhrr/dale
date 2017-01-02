#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/unquote-null-nodes.dt -o unquote-null-nodes `;
chomp for @res;
is_deeply(\@res, [], 'No compilation errors');
@res = `./unquote-null-nodes`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, ['asdf qwer'],
        'Got correct results');

`rm unquote-null-nodes`;

1;
