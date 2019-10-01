#!/usr/bin/perl
use strict;
use warnings;

# F4
my @values = (-315.334, -134.198, 232.459, 757.206, 1403.68, 2180.07, 3096.19, 4155.84, 5379.66, 6672.55);
# F5
# my @values = (-179.883, -179.669, -179.229, -178.384, -176.951, -174.1, -168.606, -159.325, -144.115, -122.825);

foreach my $D (1000, 2000, 3000, 4000, 5000, 6000, 7000, 8000, 9000, 10000) {
  print "$D variables\n";
  my $index = $D / 1000 - 1;
  foreach my $th (0, 0.2, 0.4, 0.6, 0.8) {
  # F4
  my $result = `./getEvaluationsHV.pl $values[$index] /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4_MultiObjNearestDistThreshold_${th}_Mono_F4_${D}_5_ | head -15 | tail -1`;
  # F5
  # my $result = `./getEvaluationsHV.pl $values[$index] /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5_MultiObjNearestDistThreshold_${th}_Mono_F5_${D}_5_ | head -15 | tail -1`;
  print $result;
  }
  # F4 
  my $result = `./getEvaluationsHV.pl $values[$index] experimentsResults/Bolido_Parallel_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4/Bolido_Parallel_MonoBenchmarks_EVOLVE_VarScal2_Mono_F4_algorithms_Mono_F4_${D}_2_eval_2500000_MonoWeightAvgImp_1000000_0.1_100_0_1_2_2_ | head -15 | tail -1`;
  # F5
  # my $result = `./getEvaluationsHV.pl $values[$index] experimentsResults/Bolido_Parallel_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5/Bolido_Parallel_MonoBenchmarks_EVOLVE_VarScal2_Mono_F5_algorithms_Mono_F5_${D}_2_eval_2500000_MonoWeightAvgImp_1000000_0.1_100_0_1_2_2_ | head -15 | tail -1`;
  print $result;
}
