#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-struct.dt -o ./t.dtm-struct-user-macro.o -c -m ./dtm-struct-macro`;
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

`rm libdtm-struct-macro.so`;
`rm libdtm-struct-macro-nomacros.so`;
`rm libdtm-struct-macro.dtm`;
`rm libdtm-struct-macro.bc`;
`rm libdtm-struct-macro-nomacros.bc`;
`rm dtm-struct-user-macro`;
`rm t.dtm-struct-user-macro.o`;

1;
