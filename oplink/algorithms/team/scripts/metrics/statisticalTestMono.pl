#!/usr/bin/perl

@popSize = ("5", "10", "20");
@dimensions = (50, 500);

for my $i (1..11){
	for my $k (@dimensions){
		for my $j (@popSize){
			printf("===== Problema $i Dimensiones $k Poblacion $j\n");
			system("./statisticalTests.pl 30  /tmp/Multi_Mono_F${i}_${k}_${j}.rld  /tmp/Mono_Mono_F${i}_${k}_${j}.rld ")
		}
	}
}

