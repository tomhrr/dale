#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/struct-member-type.dt -o struct-member-type `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..4];

is_deeply(\@res, [
    'int',
    'char',
    'int',
    'char',
    'int',
], 'Got expected compilation results');

@res = `./struct-member-type`;
is($?, 0, 'Program executed successfully');

`rm struct-member-type`;

1;
