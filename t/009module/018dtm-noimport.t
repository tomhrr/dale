#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More;
use IPC::Run qw(run);

plan tests => 2;

my @res = 
    map { chomp; $_ } 
        `dalec -O0 -o ./t.dt.o -c $test_dir/t/src/dtm-noimport.dt`;
ok((not @res), 'No compilation errors');
@res = map { chomp; $_ } `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-noimport-user.dt 2>&1`;
is_deeply(\@res,
["$test_dir/t/src/dtm-noimport-user.dt:8:19: error: variable not in scope: 'y'"],
        'Got correct compilation errors');

`rm libdtm-noimport.bc`;
`rm libdtm-noimport.dtm`;

1;
