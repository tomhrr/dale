#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/set-erase-value.dt -o set-erase-value `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./set-erase-value`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [ 
'0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64 65 66 67 68 69 70 71 72 73 74 75 76 77 78 79 80 81 82 83 84 85 86 87 88 89 90 91 92 93 94 95 96 97 98 99 ',
'1 2 3 5 6 7 9 10 11 13 14 15 17 18 19 21 22 23 25 26 27 29 30 31 33 34 35 37 38 39 41 42 43 45 46 47 49 51 53 54 55 57 58 59 61 62 63 65 66 67 69 70 71 73 74 75 77 78 79 81 82 83 85 86 87 89 90 91 93 94 95 97 98 99 ',
'Unable to erase non-existent element',
],
        'Got correct results');

`rm set-erase-value`;

1;
