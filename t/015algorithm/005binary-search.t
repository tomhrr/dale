#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/binary-search.dt -o binary-searchd -l pthread `;
is(@res, 0, 'No compilation errors');

@res = `./binary-searchd`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
[
'5, 5, 6, 6, 7, 7, 8, 8, 9, 9, 10, 10, ',
'1, 1, 2, 2, 3, 3, 4, 4, 5, 5, ',
'5, 5, ',
'0 is present? No',
'1 is present? Yes',
'2 is present? Yes',
'3 is present? Yes',
'4 is present? Yes',
'5 is present? Yes',
'6 is present? Yes',
'7 is present? Yes',
'8 is present? Yes',
'9 is present? Yes',
'10 is present? Yes',
'11 is present? No',
'12 is present? No',
'13 is present? No',
'14 is present? No',
'15 is present? No',
'16 is present? No',
'17 is present? No',
'18 is present? No',
'19 is present? No',
],
    'Got expected results');

`rm binary-searchd`;

1;
