#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More;

plan tests => 2;

my @res = 
    map { chomp; $_ } 
        `dalec -O0 -o ./t.dtm-noimport-int.o -c $test_dir/t/src/dtm-noimport-int.dt`;
ok((not @res), 'No compilation errors');
@res = map { chomp; $_ } `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-noimport-user2.dt 2>&1`;
is_deeply(\@res,
["$test_dir/t/src/dtm-noimport-user2.dt:1:1: error: module 'dtm-noimport-int' does not provide the following forms: z, zz",
 "$test_dir/t/src/dtm-noimport-user2.dt:1:1: error: unable to load module 'dtm-noimport-int'"],
        'Got correct compilation errors');

`rm libdtm-noimport-int.bc`;
`rm libdtm-noimport-int.dtm`;
`rm libdtm-noimport-int.so`;
`rm libdtm-noimport-int-nomacros.bc`;
`rm libdtm-noimport-int-nomacros.so`;
`rm t.dtm-noimport-int.o`;

1;
