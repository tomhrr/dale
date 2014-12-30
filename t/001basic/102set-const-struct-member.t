#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/set-const-struct-member.dt -o set-const-struct-member`;
is(@res, 0, 'No compilation errors');

@res = `./set-const-struct-member`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
'asdf'
], 'Got expected results');

`rm set-const-struct-member`;

1;
