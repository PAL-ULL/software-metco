#!/usr/bin/perl
use warnings;
use strict;

my $problem = shift || die "Error: Se debe especificar el numero del problema (Ej: 14)";

my %medianValues;

for (my $thr = 0; $thr <= 1; $thr += 0.02) {
  # Formato con dos decimales
  $thr = sprintf("%.2f", $thr) if ($thr != 0);

  # Nombre del fichero
  my $filename = "experimentsResults/Bolido_MonoBenchmarks_CEC12_Mono_F${problem}/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_CEC12_Mono_F${problem}_MultiObjNearestDistThreshold_${thr}_Mono_F${problem}_500_5.medHV.evolution";

  # Lectura del fichero
  my @fileLines = <FILE> if (open FILE, "<$filename");
  close FILE;

  # Creacion del hash con claves los valores de threshold y valores las medianas del fitness
  $medianValues{$thr} = $1 if ($fileLines[@fileLines - 1] =~ /\d+\s+(.*)/);
}

# Imprime los valores de mediana del fitness teniendo en cuenta los valores
# de threshold ordenados

print "Median of the Fitness vs. Threshold Values (Ordered by the threshold)\n";
print "$_ $medianValues{$_}\n" foreach (sort {$a <=> $b} keys %medianValues);

my $i = 0;

print "Median of the Fitness vs. Threshold Values (Ordered by the median of the fitness)\n";
$i++, print "$i $_ $medianValues{$_}\n" foreach (sort {$medianValues{$a} <=> $medianValues{$b}} keys %medianValues);

print "Entropy\n";
my $counter = 1;
print "$_ $medianValues{$_} ". log($counter++)/log(2)."\n" foreach (sort {$medianValues{$a} <=> $medianValues{$b}} keys %medianValues);
