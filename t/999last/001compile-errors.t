#!/usr/bin/perl

use warnings;
use strict;
my $test_dir = $ENV{"DALE_TEST_DIR"} || ".";
$ENV{PATH} .= ":.";

use Data::Dumper;
use Test::More;

my @error_files =
    (@ARGV)
        ? (map { "$test_dir/t/error-src/$_.dt" } @ARGV)
        : (map { chomp; $_ } `ls $test_dir/t/error-src/*.dt`);

plan tests => (@error_files * 2);
@error_files = sort @error_files;

my $opts = "-o test-error   ";

for my $file (@error_files) {
    my ($filename) = ($file =~ /.*\/(.*)$/);
    my @res = map { chomp; $_ } `dalec $ENV{"DALE_TEST_ARGS"} $file $opts 2>&1`; 
    ok(@res, 'Got compilation errors');
    open my $fh, '<', $file.'.errors' or die $!;
    my $data = do { local $/; <$fh> };
    close $fh;
    my @errors = split /\n/, $data;
    for my $error (@errors) {
        $error =~ s/^\./$test_dir/;
    }
    is_deeply(\@res, \@errors, 
              "Got correct compilation errors ($filename)");
}

1;
