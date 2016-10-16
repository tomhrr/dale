#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
#use Test::More tests => 8;
use Test::More skip_all => "problems";

my @res = `dalec $ENV{"DALE_TEST_ARGS"} --print-expansions $test_dir/t/src/eval-expression.dt -o eval-expression  `;

ok((grep { /expansion: \(pow' 2 1\) -> 2/ } @res),
    'Found first short-circuit expansion');
ok((grep { /expansion: \(pow' 2 2\) -> \(\* 2 2\)/ } @res),
    'Found second short-circuit expansion');
ok((grep { /expansion: \(pow' 2 3\) -> \(\* 2 2 2\)/ } @res),
    'Found third short-circuit expansion');
ok((grep { /expansion: \(pow' 2 4\) -> \(pow 2 4\)/ } @res),
    'Found first non-short-circuit expansion');
ok((grep { /expansion: \(pow' 2 n\) -> \(pow 2 n\)/ } @res),
    'Found second non-short-circuit expansion');
ok((grep { /unable to evaluate/ } @res),
    'Found note about second non-short-circuit expansion');

@res = `./eval-expression`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(2 4 8 16 32)], 'Got expected results');

`rm eval-expression`;

1;
