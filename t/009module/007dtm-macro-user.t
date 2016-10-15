#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -O0 $test_dir/t/src/dtm-macro.dt -o ./t.dtm-macro-user.o -c -m ./dtm-macro`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-macro-user.dt -o dtm-macro-user `;
chomp for @res;
is_deeply(\@res, [ 'Macro action' ], 
    'No compilation errors (got macro output)');

@res = `./dtm-macro-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [],
    'Got correct results (none)');

`rm libdtm-macro.so`;
`rm libdtm-macro-nomacros.so`;
`rm libdtm-macro.dtm`;
`rm libdtm-macro.bc`;
`rm libdtm-macro-nomacros.bc`;
`rm dtm-macro-user`;
`rm t.dtm-macro-user.o`;

1;
