#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 3;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} $test_dir/t/src/md-arrays.dt -o md-arrays  `;
is(@res, 0, 'No compilation errors');

@res = `./md-arrays`;
is($?, 0, 'Program executed successfully');

chomp for @res;

is_deeply(\@res, [qw(
0
1
2
3
4
5
6
7
8
9
0
0
0
0
0
0
0
0
0
0
0
1
2
3
4
5
6
7
8
9
0
2
4
6
8
10
12
14
16
18
0
3
6
9
12
15
18
21
24
27
0
4
8
12
16
20
24
28
32
36
0
5
10
15
20
25
30
35
40
45
0
6
12
18
24
30
36
42
48
54
0
7
14
21
28
35
42
49
56
63
0
8
16
24
32
40
48
56
64
72
0
9
18
27
36
45
54
63
72
81
0
0
0
0
0
0
0
0
0
0
0
0
0
1
2
0
2
4
0
0
0
0
2
4
0
4
8
)], 'Got expected results');

`rm md-arrays`;

1;
