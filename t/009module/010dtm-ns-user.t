#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-ns.dt -o ./t.dtm-ns-user.o -c -m ./dtm-ns`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-ns-user.dt -o dtm-ns-user`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-ns-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ '100', '2', '10 c 0' ],
          'Got expected results');

`rm libdtm-ns.so`;
`rm libdtm-ns-nomacros.so`;
`rm libdtm-ns.dtm`;
`rm libdtm-ns.bc`;
`rm libdtm-ns-nomacros.bc`;
`rm dtm-ns-user`;
`rm t.dtm-ns-user.o`;

1;
