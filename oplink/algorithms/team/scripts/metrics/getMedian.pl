#!/usr/bin/perl
use warnings;
use strict;

sub getMedian {
  my $file = shift;
  my $scriptR = "data1 = scan(\\\"$file\\\");
                median(data1)\n";
  my $resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
  if ($resultado =~ /\[1\] (.*)/){
    return $1;
  } else {
    printf("Error interno. Fallo en getMedian: $resultado");
    exit(-1);
  }
}

close(STDERR);

my %medianValues;
$medianValues{$_} = getMedian($_) foreach (@ARGV);

foreach my $key (sort { $medianValues{$a} <=> $medianValues{$b} } (keys %medianValues)) {
     print "$key: $medianValues{$key}\n";
}
