#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/for-cont-and-anon.dt -o for-cont-and-anon `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./for-cont-and-anon`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [
'I is 0',
'I is 1',
'1',
'i->n: 0',
'I is 2',
'I is 3',
'3',
'i->n: 0',
'i->n: 1',
'i->n: 2',
'I is 4',
'I is 5',
'5',
'i->n: 0',
'i->n: 1',
'i->n: 2',
'i->n: 3',
'i->n: 4',
'I is 6',
'I is 7',
'7',
'i->n: 0',
'i->n: 1',
'i->n: 2',
'i->n: 3',
'i->n: 4',
'i->n: 5',
'i->n: 6',
'I is 8',
'I is 9',
'9',
'i->n: 0',
'i->n: 1',
'i->n: 2',
'i->n: 3',
'i->n: 4',
'i->n: 5',
'i->n: 6',
'i->n: 7',
'i->n: 8',
],
        'Got correct results');

`rm for-cont-and-anon`;

1;
