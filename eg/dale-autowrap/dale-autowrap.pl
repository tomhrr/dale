#!/usr/bin/perl -w

use warnings;
use strict;
use File::Basename;
use JSON::XS qw(decode_json);

my %TYPE_MAP = (
    'unsigned-int'       => 'uint',
    'unsigned-char'      => 'uint8',
    'signed-char'        => 'int8',
    'short'              => '(short-type)',
    'unsigned-short'     => '(ushort-type)',
    'long'               => '(long-type)',
    'unsigned-long'      => '(ulong-type)',
    'long-long'          => '(long-long-type)',
    'unsigned-long-long' => '(ulong-long-type)',
    'ptrdiff_t'          => 'ptrdiff',
    'uint8_t'            => 'uint8',
    'uint16_t'           => 'uint16',
    'uint32_t'           => 'uint32',
    'uint64_t'           => 'uint64',
    'int8_t'             => 'int8',
    'int16_t'            => 'int16',
    'int32_t'            => 'int32',
    'int64_t'            => 'int64',
);

sub type_to_string
{
    my ($type, $imports) = @_;
    our $in_function;
    my $tag = $type->{'tag'};
    if ($tag =~ /^:/) {
        $tag =~ s/^://;
    }

    if ($tag eq 'pointer') {
        return "(p ".(type_to_string($type->{'type'}, $imports)).")";
    }
    if ($tag eq 'array') {
        if($in_function) {
          return "(p ".(type_to_string($type->{'type'}, $imports)).")";
       } else {
          return "(array-of ".$type->{'size'}." ".
                  (type_to_string($type->{'type'}, $imports)).")";
       }
    }
    if ($tag eq 'struct') {
        return $type->{'name'};
    }
    if ($tag eq 'bitfield') {
        my $bf_type = type_to_string($type->{'type'}, $imports);
        return sprintf("(bf %s %s)",
                       $bf_type,
                       $type->{'width'});
    }
    if ($tag eq 'union') {
        return $type->{'name'};
    }
    if ($tag eq 'function-pointer') {
        return "(p void)"
    }

    my $mapped_type = $TYPE_MAP{$tag};
    if ($mapped_type) {
        if ($mapped_type =~ /^\(/) {
            $imports->{'stdlib'} = 1;
        }
        return $mapped_type;
    }

    return $tag;
}

sub type_to_flat_string
{
    my ($type, $imports) = @_;

    my $str = type_to_string($type, $imports);
    $str =~ tr/() /   /;
    $str =~ s/ //g;

    return $str;
}

my %SC_MAP = (
    'static' => 'intern',
    'none'   => 'extern-c',
    'extern' => 'extern-c',
);

sub storage_class_to_string
{
    return $SC_MAP{$_[0]};
}

sub process_function
{
    my ($data, $imports) = @_;
    our $in_function = 1;

    my @params =
        map { sprintf("(%s %s)", $_->{'name'},
                                 type_to_string($_->{'type'}, $imports)) }
            @{$data->{'parameters'}};
    if (not @params) {
        @params = 'void';
    }
    my $param_str = "(".(join ' ', @params).")";

    sprintf("(def %s (fn %s %s %s))",
            $data->{'name'},
            storage_class_to_string($data->{'storage_class'}
                                 || $data->{'storage-class'}),
            type_to_string($data->{'return-type'}, $imports),
            $param_str);
#    our $in_function = 0;
}

sub process_variable
{
    my ($data, $imports) = @_;

    sprintf("(def %s (var extern %s))",
            $data->{'name'},
            type_to_string($data->{'type'}, $imports));
}

sub process_const
{
    my ($data, $imports) = @_;

    sprintf("(def %s (var intern %s))",
            $data->{'name'},
            type_to_string($data->{'type'}, $imports));
}

sub process_struct
{
    my ($data, $imports) = @_;

    my @fields =
        map { sprintf("(%s %s)", $_->{'name'},
                                 type_to_string($_->{'type'}, $imports)) }
            @{$data->{'fields'}};
    my $field_str = (@fields ? " (".(join ' ', @fields).")" : "");

    sprintf("(def %s (struct extern%s))",
            $data->{'name'},
            $field_str);
}

sub process_enum
{
    my ($data, $imports) = @_;

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
    my ($data, $imports) = @_;
    
    sprintf("(def %s (struct extern ((a %s))))",
            $data->{'name'},
            $type);
}

sub process_union
{
    my ($data, $imports) = @_;

    $imports->{'variant'} = 1;

    my $name = $data->{'name'};

    my @constructors =
        map { sprintf("(%s-%s ((value %s)))",
                      $name,
                      type_to_flat_string($_->{'type'}, $imports),
                      type_to_string($_->{'type'}, $imports)) }
            @{$data->{'fields'}};
    my $constructor_str = join ' ', @constructors;

    sprintf("(def-variant %s (%s))",
            $name,
            $constructor_str);
}

my %PROCESS_MAP = (
    function => \&process_function,
    extern   => \&process_variable,
    struct   => \&process_struct,
    const    => \&process_const,
    enum     => \&process_enum,
    typedef  => \&process_typedef,
    union    => \&process_union,
);

sub main
{
    our $in_function = 0;
    my %imports;
    my @bindings;

    while (defined (my $entry = <STDIN>)) {
        chomp $entry;
        if (($entry eq '[') or ($entry eq ']')) {
            next;
        }
        $entry =~ s/,\s*$//;
        my $data = decode_json($entry);
        if($#ARGV>=0) {
          my $path = $data->{'location'};
          my $name = fileparse($path,qr/\.[^.]*/);  
          my $arg = $ARGV[0];
          if(not ($name eq $arg)) {
            next;
          }
        }
        my $tag = $data->{'tag'};
        if ($PROCESS_MAP{$tag}) {
            push @bindings, $PROCESS_MAP{$tag}->($data, \%imports);
        } else {
            warn "unable to process tag '$tag'";
        }
    }

    my @imports = sort keys %imports;
    if (@imports) {
        for my $import (@imports) {
            print "(import $import)\n";
        }
        print "\n";
    }
    for my $binding (@bindings) {
        print "$binding\n";
    }
}

main();

1;
