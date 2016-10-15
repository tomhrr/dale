#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/implicit-funcall.dt -o implicit-funcall `;
is(@res, 0, 'No compilation errors');

@res = `./implicit-funcall`;
is($?, 0, 'Program executed successfully');

chomp for @res;
is_deeply(\@res, [
    'hello'
], 'Got expected results');

`rm implicit-funcall`;

1;
