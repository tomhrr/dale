#!/usr/bin/perl

use warnings;
use strict;

use Test::More;

my $C2FFI = $ENV{'C2FFI'} || 'c2ffi';

plan tests => 1;

my $header = 't/headers/original-first.h';
my $res = system("$C2FFI $header > original-first-output");
if ($res != 0) {
    die "$C2FFI against $header failed.";
}

my @lines =
    map { chomp; $_ }
        `cat original-first-output | ./dale-autowrap --namespace lib`;
is_deeply(\@lines,
          [split /\n/, <<EOF],
(def LIB_NAMESPACE1 (var intern int))
(def LibX (struct extern ((a int))))
(def LIB_NAMESPACE2 (var intern int))
(def libFn (fn extern-c int ((a int))))
(namespace lib 
(def Fn (fn extern int ((a int)) (libFn a)))
)
(def LIB_NAMESPACE3 (var intern int))
EOF
          "Namespace applied successfully");

unlink("original-first-output");

1;
