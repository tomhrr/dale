#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/sp-copy-fn-ret-use2.dt -o sp-copy-fn-ret-use2 `;
is_deeply(\@res, [], 'No compilation errors');
@res = `./sp-copy-fn-ret-use2`;
is($?, 0, 'Program executed successfully');

chomp for @res;

# Very similar to the previous test, only difference being that the
# SharedPtr returned by otherfn is assigned to myptr2, which is why
# the refcount remains the same on lines 2 and 4.
is_deeply(\@res, [ 'Ref count is 1',
'Ref count is 2',
'Entered other function (copied), returning it now',
'Ref count is 2',
'100',
'100',
],
        'Got correct results');

`rm sp-copy-fn-ret-use2`;

1;
