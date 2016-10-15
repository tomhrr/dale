#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 2;

my @res = `dalec -O0 -o ./t.dtm-typemap.o -c $test_dir/t/src/dtm-typemap.dt`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-typemap-user.dt 2>&1`;
chomp for @res;
is_deeply(\@res, 
          [ "$test_dir/t/src/dtm-typemap-user.dt:11:5: error: overloaded function/macro not in scope: 'myfn' (parameters are int, closest candidate expects (Pair int int))"], 
          'Got expected compilation error (typemap restored)');

`rm libpairintint.dtm`;
`rm libpairintint-nomacros.bc`;
`rm libpairintint.bc`;
`rm libpairintint.so`;
`rm libpairintint-nomacros.so`;
`rm t.dtm-typemap.o`;

1;
