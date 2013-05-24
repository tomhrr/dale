#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-struct.dt -o ./t.dt.o -c -m ./dtm-struct`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-struct-user-macro.dt -o dtm-struct-user-macro `;
chomp for @res;
is_deeply(\@res, [ '10 c 20' ], 'no compilation errors (got macro output)');

@res = `./dtm-struct-user-macro`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [],
    'Got correct results (nothing)');

`rm libdtm-struct.so`;
`rm libdtm-struct-nomacros.so`;
`rm libdtm-struct.dtm`;
`rm libdtm-struct.bc`;
`rm libdtm-struct-nomacros.bc`;
`rm dtm-struct-user-macro`;

1;
