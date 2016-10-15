#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/errno.dt -o errnod `;
is(@res, 0, 'No compilation errors');

@res = `./errnod 2>&1`;
is($?, 0, 'Program executed successfully');

chomp for @res;
$/ = chr(10);
chomp for @res;

is_deeply(\@res, [
'2',
'asdf: No such file or directory'
], 'Got expected results');

`rm errnod`;

1;
