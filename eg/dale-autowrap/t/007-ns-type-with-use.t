#!/usr/bin/perl

use warnings;
use strict;

use Test::More;

my $C2FFI = $ENV{'C2FFI'} || 'c2ffi';

plan tests => 1;

my $header = 't/headers/ns-type-with-use.h';
my $res = system("$C2FFI $header > ns-type-with-use-output");
if ($res != 0) {
    die "$C2FFI against $header failed.";
}

my @lines =
    map { chomp; $_ }
        `cat ns-type-with-use-output | ./dale-autowrap --namespace test --casing lisp`;
is_deeply(\@lines,
          [split /\n/, <<EOF],
(namespace test 
(def struct (struct extern ((a int))))
)
(def another-struct2 (struct extern ((b test.struct))))
EOF
          "Adjusted type name used throughout");

unlink("ns-type-with-use-output");

1;
