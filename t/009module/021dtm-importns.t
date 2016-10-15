#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 -o ./t.dtm-importns.o -c $test_dir/t/src/dtm-nsimport.dt`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-nsimport-user.dt -o dtm-nsimport-user`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-nsimport-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
            [ '100 200',
              '100 a 5000' ],
          'Got expected results');

`rm libdtm-nsimport.so`;
`rm libdtm-nsimport-nomacros.so`;
`rm libdtm-nsimport.dtm`;
`rm libdtm-nsimport.bc`;
`rm libdtm-nsimport-nomacros.bc`;
`rm dtm-nsimport-user`;
`rm t.dtm-importns.o`;

1;
