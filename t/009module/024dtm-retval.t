#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
#use Test::More tests => 2;
use Test::More skip_all => "problems under 3.5";

my @res = `dalec -O0 -o ./t.dtm-retval.o -c $test_dir/t/src/dtm-retval.dt`;
is_deeply(\@res, [], 'No compilation errors');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-retval-user.dt 2>&1`;
chomp for @res;
is_deeply(\@res, [], 'No compilation errors (2)');

`rm libmodretval.dtm`;
`rm libmodretval-nomacros.bc`;
`rm libmodretval.bc`;
`rm libmodretval.so`;
`rm libmodretval-nomacros.so`;
`rm t.dtm-retval.o`;
`rm a.out`;

1;
