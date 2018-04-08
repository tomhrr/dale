#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec -c $test_dir/t/src/dtm-preserve-imported1.dt`;
is_deeply(\@res, [], 'No compilation errors (1)');
@res = `dalec -c $test_dir/t/src/dtm-preserve-imported2.dt`;
is_deeply(\@res, [], 'No compilation errors (2)');

@res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/preserve-imported3.dt 2>&1`;
chomp for @res;
is(@res, 1, 'Got one compilation error');
like($res[0], qr/error: not in scope: 'mfn2'/,
    'Function not imported into second module is not available');

`rm libdtm-preserve-imported*`;

1;
