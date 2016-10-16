#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
#use Test::More tests => 3;
use Test::More skip_all => "dispatch problems";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/and-or.dt -o and-or  `;
is(@res, 0, 'No compilation errors');

@res = `./and-or`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'Trying to create object -1',
'Trying to create object 0',
'Trying to create object 1',
'Trying to create object -5',
'Trying to create object 12',
'Trying to create object 3',
'Some valid object has been created',
'No valid object has been created',
], 'Got expected results');

`rm and-or`;

1;
