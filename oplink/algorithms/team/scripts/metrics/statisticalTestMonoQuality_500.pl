#!/usr/bin/perl

@Mono_5 =  (1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_10 = (1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_20 = (1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1);
@valores = ("-449.913", "-425.905", "11277.3", "-327.898", "-179.887", "-139.98", "0.201172", "230381000000", "254.642", "1.07254", "251.536");
@Multi_5 = ("Nearest", "Avg", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");
@Multi_10 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");
@Multi_20 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");

@variables = (500);
foreach my $variable (@variables){
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 5
		printf("Variables: $variable - Problema $problema - PopSize: 5\n");
		system("./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_5[$indice]Dist_Mono_F${problema}_${variable}_5_250000.allHV /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_5[$indice]_Mono_F${problema}_${variable}_5_250000.allHV");
		printf("*************************************************\n");
		#Pop 10 
		printf("Variables: $variable - Problema $problema - PopSize: 10\n");
		system("./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_10[$indice]Dist_Mono_F${problema}_${variable}_10_250000.allHV /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_10[$indice]_Mono_F${problema}_${variable}_10_250000.allHV");
		printf("*************************************************\n");
		#Pop 20 
		printf("Variables: $variable - Problema $problema - PopSize: 20\n");
		system("./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_20[$indice]Dist_Mono_F${problema}_${variable}_20_250000.allHV /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_20[$indice]_Mono_F${problema}_${variable}_20_250000.allHV");
		printf("*************************************************\n");

	}
}
