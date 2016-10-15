#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/up-move-fn-ret.dt -o up-move-fn-ret `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./up-move-fn-ret`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'0 100 0',
'100',
'0 0 100',
],
'Got correct results');

`rm up-move-fn-ret`;

1;
