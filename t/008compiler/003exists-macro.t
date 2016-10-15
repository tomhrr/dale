#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 4;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/exists-macro.dt -o exists-macro   `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..3];

is_deeply(\@res, [
    '(myidentity int) The macro exists',
    '(myidentity) The macro exists',
    '(myidentity int (p DNode) (p DNode)) The macro exists',
    '(not-a-real-macro int) The macro does not exist',
], 'Got expected compilation results');

@res = `./exists-macro`;
is($?, 0, 'Program executed successfully');
chomp for @res;

is_deeply(\@res, [ 
    'myidentity macro exists',
    'myidentity macro exists (2)',
    'myidentity macro exists (3)',
    'not-a-real-macro does not exist',
], 'Got expected results');

`rm exists-macro`;

1;
