#!/usr/bin/env perl
# Regenerates the engine API lists in Ethanon/Ethanon.sublime-syntax from what the engine actually registers with
# AngelScript (RegisterGlobalFunction, RegisterObjectType, RegisterEnum, RegisterEnumValue, RegisterGlobalProperty in
# toolkit/Source/src/engine and src/addons). Run it after binding something new:
#     perl update-api-lists.pl
use strict;
use warnings;
use File::Basename qw(dirname);
use File::Find qw(find);

my $here = dirname(__FILE__);
my $src = "$here/../../../Source/src";
my $syntax = "$here/Ethanon/Ethanon.sublime-syntax";
die "engine sources not found at $src\n" unless -d "$src/engine";

my $code = '';
find(sub {
	return unless /\.(cpp|h|hpp)$/;
	open(my $fh, '<', $_) or die "$File::Find::name: $!\n";
	local $/;
	$code .= <$fh>;
}, "$src/engine", "$src/addons");

my (%functions, %types, %constants);
$functions{$1} = 1 while $code =~ /RegisterGlobalFunction\(\s*"[^"]*?([A-Za-z_]\w*)\s*\(/g;
$types{$1} = 1 while $code =~ /RegisterObjectType\(\s*"([A-Za-z_]\w*)/g;
$types{$1} = 1 while $code =~ /RegisterEnum\(\s*"([A-Za-z_]\w*)"/g;
$constants{$1} = 1 while $code =~ /RegisterEnumValue\(\s*"[^"]*"\s*,\s*"([A-Za-z_]\w*)"/g;
$constants{$1} = 1 while $code =~ /RegisterGlobalProperty\(\s*"[^"]*?([A-Za-z_]\w*)"/g;

die "nothing found; did the registration code move?\n" unless %functions && %types;

sub alternation { return "'(?:" . join('|', sort { $a cmp $b } keys %{$_[0]}) . ")'" }

my $block = "  # BEGIN GENERATED API LISTS (update-api-lists.pl)\n"
	. "  ethanon_functions: " . alternation(\%functions) . "\n"
	. "  ethanon_types: " . alternation(\%types) . "\n"
	. "  ethanon_constants: " . alternation(\%constants) . "\n"
	. "  # END GENERATED API LISTS\n";

open(my $in, '<', $syntax) or die "$syntax: $!\n";
my $text = do { local $/; <$in> };
close($in);
$text =~ s/^  # BEGIN GENERATED API LISTS.*?^  # END GENERATED API LISTS\n/$block/ms
	or die "markers not found in $syntax\n";
open(my $out, '>', $syntax) or die "$syntax: $!\n";
print $out $text;
close($out);

printf "%d functions, %d types, %d constants written to %s\n",
	scalar(keys %functions), scalar(keys %types), scalar(keys %constants), $syntax;
