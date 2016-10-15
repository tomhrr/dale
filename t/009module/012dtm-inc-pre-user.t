#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-inc.dt -o ./t.dtm-inc-pre-user.o -c -m ./dtm-inc-pre`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-inc-pre-user.dt -o dtm-inc-pre-user`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-inc-pre-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ 'Hello', 'Hello' ],
          'Got expected results');

`rm libdtm-inc-pre.so`;
`rm libdtm-inc-pre-nomacros.so`;
`rm libdtm-inc-pre.dtm`;
`rm libdtm-inc-pre.bc`;
`rm libdtm-inc-pre-nomacros.bc`;
`rm dtm-inc-pre-user`;
`rm t.dtm-inc-pre-user.o`;

1;
