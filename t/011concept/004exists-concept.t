#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 1;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/exists-concept.dt -o exists-concept `;
chomp for @res;
is_deeply(\@res, [
    'mycm does not exist as a concept macro',
    'mycm exists as a concept macro'
], 'Got correct compilation notices');

`rm exists-concept`;

1;
