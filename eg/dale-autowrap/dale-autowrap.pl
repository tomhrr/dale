#!/usr/bin/perl

use warnings;
use strict;

use JSON::XS qw(decode_json);

my %TYPEMAP = (
    'unsigned-int'       => 'uint',
    'unsigned-char'      => 'uint8',
    'signed-char'        => 'int8',
    'short'              => '(short-type)',
    'unsigned-short'     => '(ushort-type)',
    'long'               => '(long-type)',
    'unsigned-long'      => '(ulong-type)',
    'long-long'          => '(long-long-type)',
    'unsigned-long-long' => '(ulong-long-type)',
);

my %SCMAP = (
    'static' => 'intern',
    'none'   => 'extern-c',
    'extern' => 'extern-c',
);

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
    if ($tag eq 'array') {
        return "(array-of ".$type->{'size'}." ".
                (type_to_string($type->{'type'})).")";
    }
    if ($tag eq 'struct') {
        return $type->{'name'};
    }
    if ($tag eq 'bitfield') {
        my $bf_type = type_to_string($type->{'type'});
        return sprintf("(bf %s %s)",
                       $bf_type,
                       $type->{'width'});
    }

    my $mapped_type = $TYPEMAP{$tag};
    if ($mapped_type) {
        return $mapped_type;
    }

    return $tag;
}

sub storage_class_to_string
{
    return $SCMAP{$_[0]};
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
            storage_class_to_string($data->{'storage_class'}
                                 || $data->{'storage-class'}),
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

sub process_const
{
    my ($data) = @_;

    sprintf("(def %s (var intern %s))",
            $data->{'name'},
            type_to_string($data->{'type'}));
}

sub process_struct
{
    my ($data) = @_;

    my @fields =
        map { sprintf("(%s %s)", $_->{'name'}, type_to_string($_->{'type'})) }
            @{$data->{'fields'}};
    my $field_str = (@fields ? " (".(join ' ', @fields).")" : "");

    sprintf("(def %s (struct extern%s))",
            $data->{'name'},
            $field_str);
}

sub process_enum
{
    my ($data) = @_;

    my @fields =
        map { sprintf("(%s %s)", $_->{'name'}, $_->{'value'}) }
            @{$data->{'fields'}};
    my $field_str = (@fields ? " (".(join ' ', @fields).")" : "");

    sprintf("(def-enum %s extern int%s)",
            $data->{'name'},
            $field_str);
}

sub process_typedef
{
    my ($data) = @_;

    sprintf("(def %s (struct extern ((a %s))))",
            $data->{'name'},
            type_to_string($data->{'type'}));
}

sub main
{
    print "(import stdlib)\n";

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
        } elsif ($tag eq 'struct') {
            my $str = process_struct($data);
            print "$str\n";
        } elsif ($tag eq 'const') {
            my $str = process_const($data);
            print "$str\n";
        } elsif ($tag eq 'enum') {
            my $str = process_enum($data);
            print "$str\n";
        } elsif ($tag eq 'typedef') {
            my $str = process_typedef($data);
            print "$str\n";
        }
    }
}

main();

1;
