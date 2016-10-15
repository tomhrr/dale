#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/ctype.dt -o ctype `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./ctype`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ 'Right',
        'Right',
        'Wrong',
        'Wrong',
        'Wrong',
        'Wrong',
        'Right',
        'Wrong',
        'Wrong',
        'Wrong',
        'Wrong',
        'Right',
        'Right', ],
          'Got correct results');

`rm ctype`;

1;
