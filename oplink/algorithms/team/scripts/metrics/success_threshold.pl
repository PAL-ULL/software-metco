#!/usr/bin/perl
use warnings;
use strict;

die "Error: ./sucessThreshold.pl numProblema valorCalidad" if (@ARGV != 2);
my ($problem, $qValue) = @ARGV;

my %successValues;

for (my $thr = 0; $thr <= 1; $thr += 0.02) {
  # Formato con dos decimales
  $thr = sprintf("%.2f", $thr) if ($thr != 0);

  # Nombre del fichero
  my $filename = "experimentsResults/Bolido_MonoBenchmarks_CEC12_Mono_F${problem}/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_CEC12_Mono_F${problem}_MultiObjNearestDistThreshold_${thr}_Mono_F${problem}_500_5_";

  # Ejecutamos el comando y nos quedamos con las lineas que casan con la expresion regular
  my @resultLines = grep {/^\d+\s+.*/} `./getEvaluationsHV.pl $qValue $filename`;

  # Hash con claves los valores de threshold y valores los valores de success
  $successValues{$thr} = 0;
  $successValues{$thr} = $1 * 100.0 if ($resultLines[@resultLines - 1] =~ /\d+\s+(.*)/);
}

# Imprime los valores de success teniendo en cuenta los valores
# de threshold ordenados
print "$_ $successValues{$_}\n" foreach (sort {$a <=> $b} keys %successValues);
