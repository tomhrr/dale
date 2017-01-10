#!/usr/bin/perl

use warnings;
use strict;

use JSON::XS qw(decode_json);

sub type_to_string
{
    my ($type) = @_;

    my $tag = $type->{'tag'};
    if ($tag =~ /^:/) {
        $tag =~ s/^://;
    }

    if ($tag eq 'pointer') {
        return "(p ".(type_to_string($type->{'type'})).")";
    }

    return $tag;
}

sub process_function
{
    my ($data) = @_;

    my @params =
        map { sprintf("(%s %s)", $_->{'name'}, type_to_string($_->{'type'})) }
            @{$data->{'parameters'}};
    if (not @params) {
        @params = 'void';
    }
    my $param_str = "(".(join ' ', @params).")";

    sprintf("(def %s (fn %s %s %s))",
            $data->{'name'},
            $data->{'storage_class'},
            type_to_string($data->{'return-type'}),
            $param_str);
}

sub main
{
    while (defined (my $entry = <>)) {
        chomp $entry;
        if ($entry eq '[') {
            next;
        }
        if ($entry eq ']') {
            next;
        }
        $entry =~ s/,\s*$//;
        my $data = decode_json($entry);
        if ($data->{'tag'} eq 'function') {
            my $str = process_function($data);
            print "$str\n";
        }
    }
}

main();

1;
