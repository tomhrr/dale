#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More tests => 9;

open my $fh, '<', 'Config.h' or die $!;
my $llvm_bin_dir;
while (defined (my $line = <$fh>)) {
    chomp $line;
    if ($line =~ /LLVM_BIN_DIR/) {
        ($llvm_bin_dir) = ($line =~ /.*\s+"(.*)"$/);
    }
}
if (not $llvm_bin_dir) {
    die "Unable to get LLVM binary directory from Config.h.";
}
close $fh;

my @res = `dalec $ENV{"DALE_TEST_ARGS"} -s ir --static-modules $test_dir/t/src/empty-main.dt -o sm.dt.ll`;
is($?, 0, 'Compiled test to IR successfully (static-modules)');
is_deeply(\@res, [], 'No output from compilation');
@res = `$llvm_bin_dir/llc sm.dt.ll`;
is($?, 0, 'Compiled IR to executable successfully (using llc)');
is_deeply(\@res, [], 'No output from compilation');

@res = `dalec $ENV{"DALE_TEST_ARGS"} -s ir --static-module drt $test_dir/t/src/empty-main.dt -o sm2.dt.ll`;
is($?, 0, 'Compiled test to IR successfully (static-module)');
is_deeply(\@res, [], 'No output from compilation');
@res = `diff sm.dt.ll sm2.dt.ll`;
is_deeply(\@res, [], 'static-module gives same result as static-modules');
@res = `$llvm_bin_dir/llc sm2.dt.ll`;
is($?, 0, 'Compiled IR to executable successfully (using llc)');
is_deeply(\@res, [], 'No output from compilation');

system("rm sm.dt.ll sm2.dt.ll");

1;
