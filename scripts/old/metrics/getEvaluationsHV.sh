echo "KNAP_500_2"
echo "Seq_SPEA"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/SPEA_JP08_knap_500_2_0.01_0.9_100_100_
echo "Seq_SPEA2"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/SPEA2_JP08_knap_500_2_0.01_0.9_100_100_
echo "Seq_NSGA2"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/NSGA2_JP08_knap_500_2_0.01_0.9_100_
echo "Seq_IBEA"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/IBEA_JP08_knap_500_2_0.01_0.9_100_0.002_
echo "Seq_IBEA_Adapt"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/IBEA_Adapt_JP08_knap_500_2_0.01_0.9_100_0.002_
echo "Team"
./getEvaluationsHV.pl 0.626 experimentsResults/JP08_knap_500_2/JP08_knap_500_2_algorithms_6_probability_100_0.05_4_120_100_
