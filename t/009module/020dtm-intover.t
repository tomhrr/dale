#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 -o ./t.dtm-intover.o -c $test_dir/t/src/dtm-noimport-intover.dt`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-intover-user.dt -o dtm-intover-user --static-modules`;
chomp for @res;
is_deeply(\@res, 
          [ ], 
          'no compilation errors');

@res = `./dtm-intover-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, 
          [ '100 200 1000' ],
          'Got expected results');

`rm libdtm-noimport-intover.so`;
`rm libdtm-noimport-intover-nomacros.so`;
`rm libdtm-noimport-intover.dtm`;
`rm libdtm-noimport-intover.bc`;
`rm libdtm-noimport-intover-nomacros.bc`;
`rm dtm-intover-user`;
`rm t.dtm-intover.o`;

1;
