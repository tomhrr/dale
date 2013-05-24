#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/proper-typeof.dt -o proper-typeof`;

is($?, 0, 'Program compiled successfully');

chomp for @res;
@res = @res[0..3];

# Used to have only three 'int's here, but the first internal call
# (i.e. call required by function resolution) to my-type-of will
# succeed (literal integer), so it will be called successfully four
# times. (Once function resolution is a bit tidier (e.g. there is a
# single macro with this name and no functions, so just try that) then
# the number of times this is called may change.)
is_deeply(\@res, [qw(int int int int)],
'Got expected compilation results');

@res = `./proper-typeof`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [ '0 0 1' ],
 'Got expected results');

`rm proper-typeof`;

1;
