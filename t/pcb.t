#!/use/bin/perl

use warnings;
use strict;
use Test::More;

my $verbose = @ARGV > 0 && $ARGV[0] eq "--verbose";
my $type = "pcb";

sub run {
    my $cmd = "../cb2util @_";
    print "$cmd\n" if $verbose;
    my $result = `$cmd 2>&1`;
    print "$result" if $verbose;
    $?
}

my $out = "/dev/null";

foreach (glob "$type/*.bin") {
    is(run("-t $type $_ $out"),     0, "($_) decrypt file");
    is(run("-t $type -s $_ $out"),  0, "($_) decrypt file and strip header");
    is(run("-t $type -e $_ $out"),  0, "($_) convert to ELF");
    is(run("-t $type -c $_"),       0, "($_) verify signature");
}

done_testing();
