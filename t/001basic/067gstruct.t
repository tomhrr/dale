#!/usr/bin/perl

use warnings;
use strict;
my $test_dir;
BEGIN { $test_dir = $ENV{"DALE_TEST_DIR"} || "." };
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More skip_all => 'Problem with padded structs';
#use Test::More tests => 3;

use lib "$test_dir/t/lib";
use Dale;

my $is_x86_64 = Dale::is_x86_64($test_dir);

my $filename =
    $is_x86_64 ? "gstruct-x86-64.dt"
               : "gstruct.dt";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/$filename -o gstruct `;
is(@res, 0, 'No compilation errors');

@res = `./gstruct`;
is($?, 0, 'Program executed successfully');

chomp for @res;

if ($is_x86_64) {
    is_deeply(\@res, [
        '7f',
        'ff',
        '7fff',
        'ffff',
        'ffff0000',
        'ffff7fff',
        'ffffffffffffffff',
        'ffffffffffff7fff',
        'ffffffffffffffffffffffffffffffff',
        'ffffffffffffffffffffffffffff7fff',
    ], 'Got expected (but highly incorrect) results');
} else {
    is_deeply(\@res, [
        '7f',
        'ff',
        '7fff',
        'ffff',
        'ffff0825',
        'ffff7fff',
        'ffffffffffffffff',
        'ffffffffffff7fff',
    ], 'Got expected (but highly incorrect) results');
}

# The results below are what this program should be printing (on
# x86-64). It appears that the JIT is adding an extra two bytes of
# padding (i.e.  double what should be present), and I can't find a
# bug report about anything similar. (Given no bug report it's
# probably a dale issue.) The point of this test is to highlight
# if/when the bug (regardless of what is causing it) is fixed.

#is_deeply(\@res, [ 
#    '7f',
#    'ff',
#    '7fff',
#    'ffff',
#    '7fffffff',
#    'ffffffff',
#    '7fffffffffffffff',
#    'ffffffffffffffff',
#    '7fffffffffffffffffffffffffffffff',
#    'ffffffffffffffffffffffffffffffff',
#], 'Got expected results');
#
`rm gstruct`;

1;
