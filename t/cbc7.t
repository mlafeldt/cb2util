#!/use/bin/perl

use warnings;
use strict;
use Test::More;

my $verbose = @ARGV > 0 && $ARGV[0] eq "--verbose";
my $type = "cbc7";

sub run {
    my $cmd = "../cb2util @_";
    print "$cmd\n" if $verbose;
    my $result = `$cmd 2>&1`;
    print "$result" if $verbose;
    $?
}

foreach (glob "$type/*.cbc") {
    is(run("-t $type $_"),      0, "($_) extract cheats");
    is(run("-t $type -d $_"),   0, "($_) extract and decrypt cheats");
    isnt(run("-t $type -c $_"), 0, "($_) has no signature");
}

done_testing();
