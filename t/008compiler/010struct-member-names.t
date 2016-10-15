#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/struct-member-names.dt -o struct-member-names `;

is($?, 0, 'Program compiled successfully');


chomp for @res;
@res = @res[0..4];

is_deeply(\@res, [
    qw(n m n m r)
], 'Got expected compilation results');

@res = `./struct-member-names`;
is($?, 0, 'Program executed successfully');

`rm struct-member-names`;

1;
