#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/eacces.dt -o eacces `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./eacces`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res,
      [ 'Could not open /etc/passwd for write: permission denied' ],
          'Got correct results');

`rm eacces`;

1;
