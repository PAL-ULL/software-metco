#!/usr/bin/perl

@Mono_5 =  (1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_10 = (1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_20 = (1, 2, 1, 0, 1, 1, 1, 1, 1, 1, 1);
@valores = ("-449.913", "-425.905", "11277.3", "-327.898", "-179.887", "-139.98", "0.201172", "230381000000", "254.642", "1.07254", "251.536");
@optimos = (-450, -450, 390, -330, -180, -140, 0, 0, 0, 0, 0);
@Multi_5 = ("Nearest", "Avg", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");
@Multi_10 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");
@Multi_20 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");


close(STDERR);
@variables = (500);
foreach my $variable (@variables){
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 5
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_5[$indice]_Mono_F${problema}_${variable}_5_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}

	}
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 10 
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_10[$indice]_Mono_F${problema}_${variable}_10_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 20 
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_20[$indice]_Mono_F${problema}_${variable}_20_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 5
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_5[$indice]Dist_Mono_F${problema}_${variable}_5_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 10 
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_10[$indice]Dist_Mono_F${problema}_${variable}_10_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 20 
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_20[$indice]Dist_Mono_F${problema}_${variable}_20_2500000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
}
