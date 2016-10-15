#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/refs-no-copy-var.dt -o refs-no-copy-var`;
is(@res, 0, 'No compilation errors');

@res = `./refs-no-copy-var`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'init',
'init',
'preset',
'setf-copy-assign',
'postset',
'2',
'destroy',
'destroy',
], 'Got expected results');

`rm refs-no-copy-var`;

1;
