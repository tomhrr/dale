#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use IPC::Open3;
use Test::More tests => 1;

my ($in, $out, $err);
my $pid = open3($in, $out, $err, './daleci');
if (not $pid) {
    die "unable to run daleci";
}

print $in '(import cstdio)'."\n";
print $in '(import stdlib)'."\n";
print $in '(def x (var intern int 1))'."\n";
print $in '(while (< x 4) (do (printf "%d\n" x) (incv x)))'."\n";

<$out>;
<$out>;

my @lines;
for (1..3) {
    my $line = <$out>;
    chomp $line;
    push @lines, $line;
}

is_deeply(\@lines, [qw(1 2 3)], 'Got expected results');

1;
