#!/usr/bin/perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More;

if (not $ENV{'DALE_SLOW'}) {
    plan skip_all => '$ENV{DALE_SLOW} not set';
}
eval { require IPC::Run; };
if (my $error = $@) {
    plan skip_all => 'IPC::Run not installed';
}

# Get a list of all the programs in src.
my @progs = map { chomp; $_ } `ls $test_dir/t/src/*.dt`;

plan tests => scalar @progs * 2;

# valgrind mem-check dalec over each program.
for (@progs) {
    my @cmd = ('valgrind', '--leak-check=full', 'dalec', $_);
    my ($in, $out, $err);

    IPC::Run::run(\@cmd, \$in, \$out, \$err);

    my @res = split /\n/, $err;

    my @err_str = grep { /ERROR SUMMARY: [\d,]+ errors from [\d,]+ contexts/ } @res;
    if (not @err_str or @err_str > 1) {
        die "Could not find (single) 'errors from contexts' string!";
    }

    my ($errors, $contexts) =
        ($err_str[0] =~ /([\d,]+) errors from ([\d,]+) contexts/);

    if (not defined $errors or not defined $contexts) {
        die "Could not extract E/C from string: ".$err_str[0];
    }

    is($errors, 0, "No errors returned from memcheck for $_");
    if ($errors) {
        diag $err_str[0];
    }

    if (grep { /no leaks are possible/ } @res) {
        ok(1, "No leaks occurred\n");
    } else {
        my @deflost = grep { /definitely lost: [\d,]+ bytes in [\d,]+ blocks/ } @res;
        if (not @deflost or @deflost > 1) {
            die "Could not find (single) 'definitely lost' string!";
        }

        my ($deflost_bytes) = ($deflost[0] =~ /definitely lost: ([\d,]+) bytes/);

        is($deflost_bytes, 0, "No memory has definitely been lost");
        if ($deflost_bytes) {
            diag $deflost[0];
        }

        my @indlost = grep { /indirectly lost: [\d,]+ bytes in [\d,]+ blocks/ } @res;
        if (@indlost == 1) {
            my ($indlost_bytes) = ($indlost[0] =~ /indirectly lost: ([\d,]+) bytes/);

            is($indlost_bytes, 0, "No memory has indirectly been lost");
            if ($indlost_bytes) {
                diag $indlost[0];
            }
        }
    }

    my @mfree = grep { /malloc\/free: [\d,]+ allocs, [\d,]+ frees/ } @res;
    if (not @mfree or @mfree > 1) {
        die "Could not find (single) 'malloc/free' string!";
    }

    my $mfree1 = $mfree[0];

    my ($a, $f, $t) = ($mfree1 =~ /([\d,]+) allocs, ([\d,]+) frees, ([\d,]+) bytes allocated/);

    diag $mfree1;

    `rm $_.bc`;
}

# confirm the number of errors is 0, otherwise list the program name
# and the number of errors.

# diag the memory usage and mallocs/frees.

# make sure that either all heap memory was freed ('no leaks are
# possible') or at the very least that 0 bytes were definitely lost.
