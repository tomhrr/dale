#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/must-init.dt -o must-init  `;
is(@res, 0, 'No compilation errors');

@res = `./must-init`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
    'must initialise',
    'not necessary to initialise',
    'must initialise',
    'not necessary to initialise',
    'must initialise',
    'must initialise'
], 'Got expected results');

`rm must-init`;

1;
