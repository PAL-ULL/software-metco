#!/usr/bin/perl
use strict;
use warnings;

my %evals;

bucle: for (my $D = 1000; $D <= 10000; $D += 1000) {
  for (my $num_eval = 50000; $num_eval <= 2500000; $num_eval += 50000) {
    for (my $th1 = 0; $th1 <= 0.8; $th1 += 0.2) {
	my $diferencias = 0;
      for (my $th2 = 0; $th2 <= 0.8; $th2 += 0.2) {
        print "D = $D, num_eval = $num_eval, th1 = $th1, th2 = $th2\n";
        my $r = `./statisticalTests.pl 30 experimentsResults/Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5_MultiObjNearestDistThreshold_${th1}_Mono_F5_${D}_5_${num_eval}.allHV experimentsResults/Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5_MultiObjNearestDistThreshold_${th2}_Mono_F5_${D}_5_${num_eval}.allHV`;
        print "Resultado: $r\n\n";
        if ($r =~ /.*Mejor.*Threshold_${th1}_.*/) {
		$diferencias++;
           #printf "Ya: $D --> $num_eval\n";
		if ($diferencias == 4){
			printf("YA -> $num_eval");
          		$evals{$D} = $num_eval;
          		next bucle;
		}
        }
      }
	printf("Diferencias: $diferencias\n");
    }
  }
}

for (my $D = 1000; $D <= 10000; $D += 1000) {
  print "D = $D --> $evals{$D}\n";
}
