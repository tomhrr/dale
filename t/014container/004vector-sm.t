#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 9;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -lm $test_dir/t/src/vector.dt -o vector-big`;
is($?, 0, 'Compiled successfully');
is(@res, 0, 'No compilation errors');
@res = `dalec $ENV{"DALE_TEST_ARGS"} --static-modules -lm $test_dir/t/src/vector.dt -o vector-sm`;
is($?, 0, 'Compiled successfully (static-modules)');
is(@res, 0, 'No compilation errors');
system("rm vector-sm");

@res = `dalec $ENV{"DALE_TEST_ARGS"} -O4 --static-modules -lm $test_dir/t/src/vector.dt -o vector-small`;
is($?, 0, 'Compiled successfully (optimised)');
is(@res, 0, 'No compilation errors (optimised)');
ok(((stat('vector-big'))[7] > (stat('vector-small'))[7]),
    'Optimised static-module build yields smaller executable');

my $ret = system("./vector-small > out");
is($ret, 0, 'Program executed successfully');

@res = ();
open my $fh, '<', 'out' or die $!;
while (my $line = <$fh>) {
    push @res, $line;
}
close $fh;
chomp for @res;

is_deeply(\@res, [
'Empty?: 1',
'1 2',
'3 4',
'1 2',
'3 4',
'1 2',
'Empty?: 0',
'Empty?: 1',
'for loop',
'1 2',
'3 4',
'1 2',
'3 4',
'3 4',
'iterator',
'1 2',
'3 4',
'1 2',
'3 4',
'3 4',
'riterator',
'1 2',
'1 2',
'1 2',
'1 2',
'1 2',
'pre copy',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'post copy',
'1 2',
'1 2',
'1 2',
'1 2',
'1 2',
'pre assign from two to one',
'post assign',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'3 4',
'100',
], 'Got expected results');

`rm vector-big`;
`rm vector-small`;
`rm out`;

1;
