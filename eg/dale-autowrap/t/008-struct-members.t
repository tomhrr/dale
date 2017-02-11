#!/usr/bin/perl

use warnings;
use strict;

use Test::More;

my $C2FFI = $ENV{'C2FFI'} || 'c2ffi';

plan tests => 1;

my $header = 't/headers/struct-members.h';
my $res = system("$C2FFI $header > struct-members-output");
if ($res != 0) {
    die "$C2FFI against $header failed.";
}

my @lines =
    map { chomp; $_ }
        `cat struct-members-output | ./dale-autowrap --casing lisp`;
is_deeply(\@lines,
          [split /\n/, <<EOF],
(def test-struct (struct extern ((first-element int) (second-element int))))
EOF
          "Struct elements cased correctly");

unlink("struct-members-output");

1;
