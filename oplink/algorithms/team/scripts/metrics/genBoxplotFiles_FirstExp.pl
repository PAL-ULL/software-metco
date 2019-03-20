#!/usr/bin/perl
use warnings;
use strict;

my @problems = qw {berlin malaga paris stockholm};
my %evals = (berlin => 10000, malaga => 1000, paris => 10000, stockholm => 10000);
my %hash_numVar = (berlin => 611, paris => 448, stockholm => 445, malaga => 4761);

for my $problem (@problems) {

  my $pm = 1 / $hash_numVar{$problem};

  my $script = qq{postscript("images/FirstExp/boxplot_allHV_FirstExp_${problem}.eps", horizontal=FALSE, height=8, width=16, pointsize=12)
data1<-scan("results/FirstExp/MonoGA/Centella_MonoGA_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV")
data2<-scan("results/FirstExp/NSGA2_Multi_DCN/Centella_NSGA2_Multi_DCN_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV")
data3<-scan("results/FirstExp/NSGA2_Multi_DBI/Centella_NSGA2_Multi_DBI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV")
data4<-scan("results/FirstExp/NSGA2_Multi_ADI/Centella_NSGA2_Multi_ADI_Sumo_${problem}_FirstExp_Mutate_Pol_Crossover_Uniform_100_${pm}_1_$evals{$problem}_$evals{$problem}.allHV")
dataM<-matrix(c(data1,data2,data3,data4), 30)
library("Rlab")
bplot(dataM, space = 0.6, labels = c("MonoGA", "NSGA2_DCN", "NSGA2_DBI", "NSGA2_ADI"), ylab = "Objective value", main = "${problem} - $evals{$problem} evaluations. - 30 exec.")
dev.off()};

  open FILE, "> boxplot_allHV_FirstExp_${problem}.R";
  print FILE $script;
  close FILE;
}
