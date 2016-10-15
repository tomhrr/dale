#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 6;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/modc.dt -o ./t.dtm-rec.o -c -m ./modc`;
is_deeply(\@res, [], 'no compilation errors');
   @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/modp.dt -o ./t.dtm-rec.o -c -m ./modp`;
is_deeply(\@res, [], 'no compilation errors');
   @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/modp2.dt -o ./t.dtm-rec.o -c -m ./modp2`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/modp-user.dt -o modp-user --static-module=modc`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./modp-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ '200', '200' ],
          'Got expected results');

`rm libmodc.so`;
`rm libmodc-nomacros.so`;
`rm libmodp.so`;
`rm libmodp-nomacros.so`;
`rm libmodc.dtm`;
`rm libmodp.dtm`;
`rm libmodc.bc`;
`rm libmodc-nomacros.bc`;
`rm libmodp.bc`;
`rm libmodp-nomacros.bc`;
`rm libmodp2.bc`;
`rm libmodp2-nomacros.bc`;
`rm libmodp2.dtm`;
`rm libmodp2.so`;
`rm libmodp2-nomacros.so`;
`rm modp-user`;
`rm t.dtm-rec.o`;

1;
