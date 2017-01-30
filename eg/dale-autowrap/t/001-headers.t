#!/usr/bin/perl

use warnings;
use strict;

use Test::More;

my $C2FFI = $ENV{'C2FFI'} || 'c2ffi';

my @headers =
    map { chomp; $_ }
        `ls ./t/headers/*.h`;

plan tests => (@headers * 3);

for my $header (@headers) {
    my $res = system("$C2FFI $header > output");
    if ($res != 0) {
        die "$C2FFI against $header failed.";
    }
    $res = system("cat output | ./dale-autowrap.pl > output.dt");
    ok((not $res), "autowrap against $header succeeded");
    if ($res) {
        ok(0, "able to compile autowrap result");
    } else {
        $res = system("dalec -c output.dt");
        ok((not $res), "able to compile autowrap result");
    }

    $res = system("diff output.dt $header.dt");
    ok((not $res), "autowrapped result for $header matches expected result");
}

unlink("output");
unlink("output.dt");
unlink("output.dt.o");

1;
