#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/refs-no-copy.dt -o refs-no-copy`;
is(@res, 0, 'No compilation errors');

@res = `./refs-no-copy`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'init',
'init',
'init',
'pre-plus',
'pre fn call',
'init',
'pre fn return',
'setf-copy',
'destroy',
'setf-assign',
'destroy',
'post-plus',
'2',
'destroy',
'destroy',
'destroy',
], 'Got expected results');

if (@res) {
    print Dumper(\@res);
}

`rm refs-no-copy`;

1;
