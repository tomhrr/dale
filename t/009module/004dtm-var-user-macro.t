#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

# If a variable is modified within a macro, the modifications are only
# visible at compile time. This makes everything much simpler, though
# could change in the future if there's a compelling enough reason.

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-var.dt -o ./t.dtm-var-user-macro.o -c -m ./dtm-var-macro`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-var-user-macro.dt -o dtm-var-user-macro `;
chomp for @res;
is_deeply(\@res, [ '100', '101' ], 'No compilation errors (got macro output)');

@res = `./dtm-var-user-macro`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ '100' ],
    'Got correct results (original variable value)');

`rm libdtm-var-macro.so`;
`rm libdtm-var-macro-nomacros.so`;
`rm libdtm-var-macro.dtm`;
`rm libdtm-var-macro.bc`;
`rm libdtm-var-macro-nomacros.bc`;
`rm dtm-var-user-macro`;
`rm t.dtm-var-user-macro.o`;

1;
