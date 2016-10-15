#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/extra-parens.dt -o extra-parens `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./extra-parens`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 'extra parens'
],
        'Got correct results');

`rm extra-parens`;

1;
