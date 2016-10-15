#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec -O0 -o ./t.dash-M.o -c $test_dir/t/src/dtm-typemap-dashm.dt`;
is_deeply(\@res, [], 'No compilation errors');
mkdir 'moddir';
`mv *pairintint-dashm* moddir`;

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/dtm-typemap-user-dashm.dt 2>&1`;
ok((grep { /No such file or directory/ } @res),
    'Unable to load module');

@res = `dalec $ENV{"DALE_TEST_ARGS"} -M moddir $test_dir/t/src/dtm-typemap-user-dashm.dt 2>&1`;
chomp for @res;
is_deeply(\@res, 
          [ "$test_dir/t/src/dtm-typemap-user-dashm.dt:11:5: error: overloaded function/macro not in scope: 'myfn' (parameters are int, closest candidate expects (Pair int int))"], 
          'Got expected compilation error (typemap restored)');

`rm t.dash-M.o`;
`rm -rf moddir`;

1;
