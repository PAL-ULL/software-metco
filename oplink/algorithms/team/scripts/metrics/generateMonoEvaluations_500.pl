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
		#Multis
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_5[$indice]Dist_Mono_F${problema}_${variable}_5_ > /tmp/Multi_Mono_F${problema}_${variable}_5.rld`;
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_10[$indice]Dist_Mono_F${problema}_${variable}_10_ > /tmp/Multi_Mono_F${problema}_${variable}_10.rld`;
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_20[$indice]Dist_Mono_F${problema}_${variable}_20_ > /tmp/Multi_Mono_F${problema}_${variable}_20.rld`;
		#Monos
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_5[$indice]_Mono_F${problema}_${variable}_5_ > /tmp/Mono_Mono_F${problema}_${variable}_5.rld`;
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_10[$indice]_Mono_F${problema}_${variable}_10_ > /tmp/Mono_Mono_F${problema}_${variable}_10.rld`;
		`./getEvaluationsHV.pl $valores[$indice] experimentsResults/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_20[$indice]_Mono_F${problema}_${variable}_20_ > /tmp/Mono_Mono_F${problema}_${variable}_20.rld`;
	}
}
