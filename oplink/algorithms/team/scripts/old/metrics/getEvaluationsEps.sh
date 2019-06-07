echo "ZDT1"
echo "Hete"
./getEvaluationsEps.pl 1.01 experimentsResults/exampleZDT1/exampleZDT1_algorithms_4_island_hete_100_0.05_4_150_100_
echo "Prob"
./getEvaluationsEps.pl 1.01 experimentsResults/exampleZDT1/exampleZDT1_algorithms_4_probability_100_0.05_4_150_100_
echo "homo_nsga2"
./getEvaluationsEps.pl 1.01 experimentsResults/exampleZDT1/exampleZDT1_nsga2_4_island_homo_100_0.05_4_150_100_
echo "homo spea2"
./getEvaluationsEps.pl 1.01 experimentsResults/exampleZDT1/exampleZDT1_spea2_4_island_homo_100_0.05_4_150_100_
echo "homo spea"
./getEvaluationsEps.pl 1.01 experimentsResults/exampleZDT1/exampleZDT1_spea_4_island_homo_100_0.05_4_150_100_
