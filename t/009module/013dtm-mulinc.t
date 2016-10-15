#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 5;

my @res = `dalec -O0 $test_dir/t/src/dtm1.dt -o ./t.dtm-mulinc.o -c -m ./mod1`;
is_deeply(\@res, [], 'no compilation errors');
   @res = `dalec -O0 $test_dir/t/src/dtm2.dt -o ./t.dtm-mulinc.o -c -m ./mod2`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-mulinc.dt -o dtm-mulinc`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-mulinc`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ 'hello' ],
          'Got expected results');

`rm libmod1.so`;
`rm libmod1-nomacros.so`;
`rm libmod2.so`;
`rm libmod2-nomacros.so`;
`rm libmod1.dtm`;
`rm libmod2.dtm`;
`rm libmod1.bc`;
`rm libmod1-nomacros.bc`;
`rm libmod2.bc`;
`rm libmod2-nomacros.bc`;
`rm dtm-mulinc`;
`rm t.dtm-mulinc.o`;

1;
