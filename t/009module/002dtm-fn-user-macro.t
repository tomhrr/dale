#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-fn.dt -o t.dtm-fn-user-macro.o -c -m ./dtm-fn-macro`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-fn-user-macro.dt -o dtm-fn-user-macro `;
chomp for @res;
is_deeply(\@res, [ '100' ], 'No compilation errors (got macro output)');

@res = `./dtm-fn-user-macro`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [],
    'Got correct results (none)');

`rm libdtm-fn-macro.so`;
`rm libdtm-fn-macro-nomacros.so`;
`rm libdtm-fn-macro.dtm`;
`rm libdtm-fn-macro.bc`;
`rm libdtm-fn-macro-nomacros.bc`;
`rm dtm-fn-user-macro`;
`rm t.dtm-fn-user-macro.o`;

1;
