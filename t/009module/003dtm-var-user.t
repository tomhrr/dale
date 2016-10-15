#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-var.dt -o t.dtm-var-user.o -c -m ./dtm-var`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-var-user.dt -o dtm-var-user `;
is_deeply(\@res, [], 'no compilation errors');

@res = `./dtm-var-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ '100' ],
    'Got correct results');

`rm libdtm-var.so`;
`rm libdtm-var-nomacros.so`;
`rm libdtm-var.dtm`;
`rm libdtm-var.bc`;
`rm libdtm-var-nomacros.bc`;
`rm dtm-var-user`;
`rm t.dtm-var-user.o`;

1;
