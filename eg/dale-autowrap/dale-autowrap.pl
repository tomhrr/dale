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
    if ($tag eq 'struct') {
        return $type->{'name'};
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

sub process_variable
{
    my ($data) = @_;

    sprintf("(def %s (var extern %s))",
            $data->{'name'},
            type_to_string($data->{'type'}));
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
        my $tag = $data->{'tag'};
        if ($tag eq 'function') {
            my $str = process_function($data);
            print "$str\n";
        } elsif ($tag eq 'extern') {
            my $str = process_variable($data);
            print "$str\n";
        }
    }
}

main();

1;
