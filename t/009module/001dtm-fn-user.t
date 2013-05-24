#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-fn.dt -o ./t.dt.o -c -m ./dtm-fn`;
is_deeply(\@res, [], 'no compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-fn-user.dt -o dtm-fn-user `;
is_deeply(\@res, [], 'no compilation errors');

@res = `./dtm-fn-user`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ '100' ],
    'Got correct results');

`rm libdtm-fn.so`;
`rm libdtm-fn-nomacros.so`;
`rm libdtm-fn.dtm`;
`rm libdtm-fn.bc`;
`rm libdtm-fn-nomacros.bc`;
`rm dtm-fn-user`;

1;
