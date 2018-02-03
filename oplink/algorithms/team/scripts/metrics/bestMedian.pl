#!/usr/bin/perl
use warnings;
use strict;

my %medValues;

for my $D (1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000) {
  for my $th (0, 0.2, 0.4, 0.6, 0.8) {

    my $file = "../../results/Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4_MultiObjNearestDistThreshold_${th}_Mono_F4_${D}_5_2500000.medHV";

    # Lectura del fichero
    my @fileLines = <FILE> if (open FILE, "<$file");
    close FILE;

    # Creacion del hash con claves los valores de threshold y valores las medianas del fitness
    $medValues{$file} = $1 if ($fileLines[@fileLines - 1] =~ /(.*)/);
  }

  # Imprime los valores de mediana del fitness teniendo en cuenta los valores
  # de threshold ordenados
  # print "Median of the Cost - ${D} variables\n";
  print "$medValues{$_}, " foreach ((sort { $medValues{$a} <=> $medValues{$b} } keys %medValues)[4]);
  %medValues = ();
}
