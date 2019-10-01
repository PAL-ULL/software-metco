#!/usr/bin/perl
use warnings;
use strict;

my %avgValues;

my @files = `ls /home/csegura/svn/oplink/algorithms/team/results/Hector_2DPack_Multi_10_10/*18000*avHV`;

foreach (@files) {
  # Lectura del fichero
  my @fileLines = <FILE> if (open FILE, "<$_");
  close FILE;

  # Creacion del hash con claves los valores de threshold y valores las medianas del fitness
  $avgValues{$_} = $1 if ($fileLines[@fileLines - 1] =~ /(.*)/);
}

# Imprime los valores de mediana del fitness teniendo en cuenta los valores
# de threshold ordenados

print "Average of the Cost\n";
print "$_ $avgValues{$_}\n" foreach (sort { $avgValues{$b} <=> $avgValues{$a} } keys %avgValues);
