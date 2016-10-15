#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -o ./t.dtm-infile.o -c $test_dir/t/src/modp-infile.dt`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/modp-infile-user.dt -o modp-infile-user --static-modules`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./modp-infile-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ '20' ],
          'Got expected results');

`rm libmodp-infile.so`;
`rm libmodp-infile-nomacros.so`;
`rm libmodp-infile.dtm`;
`rm libmodp-infile.bc`;
`rm libmodp-infile-nomacros.bc`;
`rm modp-infile-user`;
`rm t.dtm-infile.o`;

1;
