#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

# If a variable is modified within a macro, the modifications are only
# visible at compile time. This makes everything much simpler, though
# could change in the future if there's a compelling enough reason.

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-var.dt -o ./t.dt.o -c -m ./dtm-var`;
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

`rm libdtm-var.so`;
`rm libdtm-var-nomacros.so`;
`rm libdtm-var.dtm`;
`rm libdtm-var.bc`;
`rm libdtm-var-nomacros.bc`;
`rm dtm-var-user-macro`;

1;
