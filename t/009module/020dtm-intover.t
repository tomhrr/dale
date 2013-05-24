#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 -c $test_dir/t/src/dtm-noimport.dt`;
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

`rm libdtm-noimport.so`;
`rm libdtm-noimport-nomacros.so`;
`rm libdtm-noimport.dtm`;
`rm libdtm-noimport.bc`;
`rm libdtm-noimport-nomacros.bc`;
`rm dtm-intover-user`;

1;
