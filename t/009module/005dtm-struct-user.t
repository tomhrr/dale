#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-struct.dt -o ./t.dtm-struct-user.o -c -m ./dtm-struct`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-struct-user.dt -o dtm-struct-user `;
is_deeply(\@res, [], 'no compilation errors');

@res = `./dtm-struct-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ '10 c 20' ],
    'Got correct results');

`rm libdtm-struct.so`;
`rm libdtm-struct-nomacros.so`;
`rm libdtm-struct.dtm`;
`rm libdtm-struct.bc`;
`rm libdtm-struct-nomacros.bc`;
`rm dtm-struct-user`;
`rm t.dtm-struct-user.o`;

1;
