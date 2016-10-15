#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/implicit-no-copy.dt -o implicit-no-copy`;
is(@res, 0, 'No compilation errors');

@res = `./implicit-no-copy`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'Preswap',
'...Postswap',
'1 2 4',
'1 2 3',
], 'Got expected results');

`rm implicit-no-copy`;

1;
