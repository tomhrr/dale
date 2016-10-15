#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 -o ./t.dtm-importforms.o -c $test_dir/t/src/dtm-importforms.dt`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-importforms-user.dt -o dtm-importforms-user`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-importforms-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ '100',
            '1000',
            '10 20 3' ],
          'Got expected results');

`rm libmod-importforms.so`;
`rm libmod-importforms-nomacros.so`;
`rm libmod-importforms.dtm`;
`rm libmod-importforms.bc`;
`rm libmod-importforms-nomacros.bc`;
`rm dtm-importforms-user`;
`rm t.dtm-importforms.o`;

1;
