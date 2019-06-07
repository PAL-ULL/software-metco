#!/usr/bin/perl

@Mono_5 =  (1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_10 = (1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1);
@Mono_20 = (1, 1, 2, 2, 1, 1, 1, 1, 1, 1, 1);
@valores = ("-449.987", "-447.17", "8735.3", "-329", "-179.847", "-139.973", "0.028113", "2997070000", "29.1965", "0.0544953", "31.6217");
@optimos = (-450, -450, 390, -330, -180, -140, 0, 0, 0, 0, 0);
@Multi_5 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Avg", "Nearest", "Nearest", "Nearest");
@Multi_10 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");
@Multi_20 = ("Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest", "Nearest");

close(STDERR);
@variables = (50);
foreach my $variable (@variables){
	foreach my $problema(1..11){
		$indice = $problema -1;
		#Pop 5
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_5[$indice]_Mono_F${problema}_${variable}_5_250000.allHV`;
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
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_10[$indice]_Mono_F${problema}_${variable}_10_250000.allHV`;
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
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/MonoGA_Bolido_MonoBenchmarks_$Mono_20[$indice]_Mono_F${problema}_${variable}_20_250000.allHV`;
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
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_5[$indice]Dist_Mono_F${problema}_${variable}_5_250000.allHV`;
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
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_10[$indice]Dist_Mono_F${problema}_${variable}_10_250000.allHV`;
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
		$valor = `./statisticalTests.pl 30 /home/edusegre/oplink/algorithms/team/results/Bolido_MonoBenchmarks_Multi/NSGA2_MultiObjectivized_Bolido_MonoBenchmarks_Multi_MultiObj$Multi_20[$indice]Dist_Mono_F${problema}_${variable}_20_250000.allHV`;
		$error = $valor - $optimos[$indice];
		if ($indice != 10){
			printf("%.3e & ", $error);
		} else {
			printf("%.3e \\\\ \\hline \n", $error);
		}
	}
}
