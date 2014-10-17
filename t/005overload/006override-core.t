#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More skip_all => 'Does not pass during "make tests".';
#use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/override-core.dt -o override-core `;
is(@res, 0, 'No compilation errors');

@res = `./override-core`;
is($?, 0, 'Program executed successfully');

chomp for @res;
shift @res;
shift @res;
is_deeply(\@res, [
    'Overrode setf for float: old -0.00, new 1.00',
    '1.00',
], 'Got expected results');

`rm override-core`;

1;
