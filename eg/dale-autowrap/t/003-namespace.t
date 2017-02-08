#!/usr/bin/perl

use warnings;
use strict;

use Test::More;

my $C2FFI = $ENV{'C2FFI'} || 'c2ffi';

plan tests => 1;

my $header = 't/headers/namespace.h';
my $res = system("$C2FFI $header > namespace-output");
if ($res != 0) {
    die "$C2FFI against $header failed.";
}

my @lines =
    map { chomp; $_ }
        `cat namespace-output | ./dale-autowrap --namespace first_`;
is_deeply(\@lines,
          [split /\n/, <<EOF],
(def first_testfn1 (fn extern-c int ((a int) (b float))))
(namespace first_ 
(def testfn1 (fn extern int ((a int) (b float)) (first_testfn1 a b)))
)
(def first_testfn2 (fn extern-c int (void)))
(namespace first_ 
(def testfn2 (fn extern int (void) (first_testfn2)))
)
(def otherfn (fn extern-c int (void)))
EOF
          "Namespace applied successfully");

unlink("namespace-output");

1;
