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
        `dalec -O0 -o ./t.dtm-opstruct.o -c $test_dir/t/src/dtm-opstruct.dt`;
ok((not @res), 'No compilation errors');
@res = map { chomp; $_ } `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-opstruct-user.dt 2>&1`;
is_deeply(\@res,
["$test_dir/t/src/dtm-opstruct-user.dt:3:8: error: cannot instantiate opaque struct",
 "$test_dir/t/src/dtm-opstruct-user.dt:5:20: error: type not in scope: 'instruct'",
 "$test_dir/t/src/dtm-opstruct-user.dt:6:23: error: type not in scope: 'instruct'"],
        'Got correct compilation errors');

`rm libdtm-opstruct.so`;
`rm libdtm-opstruct-nomacros.so`;
`rm libdtm-opstruct.bc`;
`rm libdtm-opstruct-nomacros.bc`;
`rm libdtm-opstruct.dtm`;
`rm t.dtm-opstruct.o`;

1;
