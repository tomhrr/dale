#!/usr/bin/env perl

# This test was previously ensuring that a function with typed
# parameters was preferred to a macro with untyped parameters, even
# when the relevant function was in a higher namespace.  That was
# implemented largely to support a specific implementation of
# operator-macros, but it has since been modified such that this is no
# longer necessary.  Given that some legitimate use cases are
# prevented by this behaviour (see #130), and there aren't others to
# support keeping it, it has been reverted.

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/overload-cross-ns.dt -o overload-cross-ns `;
chomp for @res;
is_deeply(\@res, ['Called macro', 'Called macro'],
     'Got expected compilation results');

@res = `./overload-cross-ns`;
is($?, 0, 'Program executed successfully');

chomp for @res;
my @final_res = map { s/\..*//; $_ } @res;
is_deeply(\@final_res, [
    '6',
    '6'
], 'Got expected results');

`rm overload-cross-ns`;

1;
