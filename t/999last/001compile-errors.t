#!/usr/bin/env perl

use warnings;
use strict;
$ENV{"DALE_TEST_ARGS"} ||= "";
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Test::More tests => 324;

my @error_files =
    (@ARGV)
        ? (map { "$test_dir/t/error-src/$_.dt" } @ARGV)
        : (map { chomp; $_ } `ls $test_dir/t/error-src/*.dt`);

@error_files = sort @error_files;

my $opts = "-o test-error   ";

for my $file (@error_files) {
    my ($filename) = ($file =~ /.*\/(.*)$/);
    my @res = map { chomp; $_ } `dalec $ENV{"DALE_TEST_ARGS"} -I$test_dir/t/error-src $file $opts 2>&1`;
    ok(@res, "Got compilation errors ($filename)");
    open my $fh, '<', $file.'.errors' or die "$file.errors: $!";
    my $data = do { local $/; <$fh> };
    close $fh;
    my @errors = split /\n/, $data;
    if (@res != @errors) {
        ok(0, "Got incorrect number of compilation errors ($filename)");
    } else {
        for (my $i = 0; $i < @errors; $i++) {
            my $error = $errors[$i];
            $error =~ s/^\.\//$test_dir\//;
            $error =~ s/\(/\\\(/g;
            $error =~ s/\)/\\\)/g;
            my $actual_error = $res[$i];
            like($actual_error, qr/$error/, 'Error pattern matches actual error');
        }
    }
}

1;
