#!/usr/bin/perl
use warnings;
use strict;

my @init_A = qw {NSGA2_DCN NSGA2_DBI NSGA2_ADI};
my @init_B = qw {NSGA2_DCN NSGA2_DBI NSGA2_ADI};
my @problems = qw {berlin malaga paris stockholm};
my %evals = (berlin => 10000, malaga => 1000, paris => 10000, stockholm => 10000);
my %hash_numVar = (berlin => 611, paris => 448, stockholm => 445, malaga => 4761);
my $stat;
my $pValues;

for my $problem (@problems) {

  my $pm = 1 / $hash_numVar{$problem};
 
  for (my $i = 0; $i < @init_A; $i++) {
    for (my $j = $i + 1; $j < @init_B; $j++) {

      my $file_A = "NSGA2_Multi_DCN/Centella_NSGA2_Multi_DCN_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_A[$i] eq "NSGA2_DCN");
      $file_A = "NSGA2_Multi_DBI/Centella_NSGA2_Multi_DBI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_A[$i] eq "NSGA2_DBI");
      $file_A = "NSGA2_Multi_ADI/Centella_NSGA2_Multi_ADI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_A[$i] eq "NSGA2_ADI");
      
      my $file_B = "NSGA2_Multi_DCN/Centella_NSGA2_Multi_DCN_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_B[$j] eq "NSGA2_DCN");
      $file_B = "NSGA2_Multi_DBI/Centella_NSGA2_Multi_DBI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_B[$j] eq "NSGA2_DBI");
      $file_B = "NSGA2_Multi_ADI/Centella_NSGA2_Multi_ADI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV" if ($init_B[$j] eq "NSGA2_ADI");
      
      my @result = `./statisticalTests_old.pl 30 $file_A $file_B`;
      my ($mean1, $mean2, $median1, $median2, $sd1, $sd2, $pValue, $testName, $eSize) = split /\s+/, $result[0];

      if (($i == 0) && ($j == 1)) {
        $stat->{$problem}{$init_A[$i]} = [$mean1, $median1, $sd1];
      }

      if ($i == 0) {
        $stat->{$problem}{$init_B[$j]} = [$mean2, $median2, $sd2];
      }

      if ($pValue < 0.05) {
        if (($mean1 < $mean2) && ($median1 < $median2)) {
          $pValues->{$problem}{$init_A[$i]}{$init_B[$j]} = [$pValue, $testName, '$\uparrow$'];
        }
        elsif (($mean1 > $mean2) && ($median1 > $median2)) {
          $pValues->{$problem}{$init_A[$i]}{$init_B[$j]} = [$pValue, $testName, '$\downarrow$'];
        }
      }
      else {
        $pValues->{$problem}{$init_A[$i]}{$init_B[$j]} = [$pValue, $testName, '$\leftrightarrow$'];
      }
    }
  }
}

for my $problem (@problems) {
  my $mean_min   = $stat->{$problem}{$init_A[0]}[0];
  my $median_min = $stat->{$problem}{$init_A[0]}[1];
  my $mean_alg = $init_A[0];
  my $median_alg = $init_A[0];
  my $line = "${problem}";

  for my $initA (@init_A) {
    if ($stat->{$problem}{$initA}[0] < $mean_min) {
      $mean_min = $stat->{$problem}{$initA}[0];
      $mean_alg = $initA;
    }
    if ($stat->{$problem}{$initA}[1] < $median_min) {
      $median_min = $stat->{$problem}{$initA}[1];
      $median_alg = $initA;
    }
  }

  for my $initA (@init_A) {
    if ($initA eq $mean_alg) {
      $line .= sprintf(' & {\\bf %.3e}', $stat->{$problem}{$initA}[0]);
    }
    else {
      $line .= sprintf(' & %.3e', $stat->{$problem}{$initA}[0]);
    }

    if ($initA eq $median_alg) {
      $line .= sprintf(' & {\\bf %.3e}', $stat->{$problem}{$initA}[1]);
    }
    else {
      $line .= sprintf(' & %.3e', $stat->{$problem}{$initA}[1]);
    }
      
    $line .= sprintf(' & %.3e', $stat->{$problem}{$initA}[2]);
  }
  $line .= ' \\\\';
  print "$line\n";
}

print "\n\n";

for my $problem (@problems) {
  my $line = "${problem}";
  for (my $i = 0; $i < @init_A; $i++) {
    for (my $j = $i + 1; $j < @init_B; $j++) {
      my $initA = $init_A[$i];
      my $initB = $init_B[$j];
      if ($pValues->{$problem}{$initA}{$initB}[2] ne '$\leftrightarrow$') {
       $line .= sprintf(' & {\\bf %.3e} & %s & %s', $pValues->{$problem}{$initA}{$initB}[0], $pValues->{$problem}{$initA}{$initB}[2], $pValues->{$problem}{$initA}{$initB}[1]);
      }
      else {
       $line .= sprintf(' & %.3e & %s & %s', $pValues->{$problem}{$initA}{$initB}[0], $pValues->{$problem}{$initA}{$initB}[2], $pValues->{$problem}{$initA}{$initB}[1]);
      }
    }
  }
  $line .= ' \\\\';
  print "$line\n";
}
