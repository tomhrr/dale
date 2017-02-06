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
    my $res = system("$C2FFI -I ./t/headers $header > output 2>/dev/null");
    if ($res != 0) {
        die "$C2FFI against $header failed.";
    }
    $res = system("$C2FFI -I ./t/headers -M pre $header >/dev/null");
    if ($res != 0) {
        die "$C2FFI for macros against $header failed.";
    }
    $res = system("$C2FFI -I ./t/headers pre > output2 2>/dev/null");
    if ($res != 0) {
        die "$C2FFI for macros against $header failed.";
    }
    my $extra = ($header =~ /\/file.h$/) ? '--file=file.h' : '';
    $res = system("cat output2 output | ./dale-autowrap $extra > output.dt");
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

unlink("pre");
unlink("output");
unlink("output2");
unlink("output.dt");
unlink("output.dt.o");

1;
