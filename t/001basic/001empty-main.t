#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/empty-main.dt -o empty-main`;
is(@res, 0, 'No compilation errors');

@res = `./empty-main`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is(@res, 0, 'Got expected results');

if (@res) {
    print Dumper(\@res);
}

`rm empty-main`;

1;
