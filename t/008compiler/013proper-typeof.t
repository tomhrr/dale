#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/proper-typeof.dt -o proper-typeof`;

is($?, 0, 'Program compiled successfully');

chomp for @res;
@res = @res[0..2];

is_deeply(\@res, [qw(int int int)],
'Got expected compilation results');

@res = `./proper-typeof`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [ '0 0 1' ],
 'Got expected results');

`rm proper-typeof`;

1;
