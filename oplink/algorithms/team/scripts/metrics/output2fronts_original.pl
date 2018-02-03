#!/usr/bin/perl

#uso output2fronts.pl name 
#Dado un fichero de salida del mainTeam o mainMOEA crea un fichero para
#cada frente llamado name_numberOfEvaluations.front para cada frente
#encontrado en el fichero
#Ademas imprime por pantalla el número de ficheros creados y el
#numero de evaluaciones de cada caso y lo guarda en un fichero.

$name = shift;
`gunzip $name.gz`;
if (!defined($name)){
	printf("Uso $0 name outputFile\n");
	exit(-1);
}

$lastEval = -1;

while(<>){
	if ($_ =~m/Current Evaluation = (\d*)/i){
		#if ($lastEval != $1){#si aparece varias veces la misma evaluacion la ignoramos (puede ocurrir en la ultima)
			$lastEval = $1;
			push(@numEvaluations, $1);
			if (!open FRENTE, ">${name}_$1.front"  ) { 
				printf("Error creating file ${name}_$1.front");
				exit(-1);
			} 
			$abierto = 1;
		#}
	}
	if ($_ =~m/Front Size = (\d*)/i){
		if ($abierto == 1){
			foreach (1..$1){
				$line = <>;
				print FRENTE "$line";
			}
			close (FRENTE);
			$abierto = 0;
		}
	}
}

if (!open EVALS, ">${name}.evaluations"){
	printf("Error creating file ${name}.evaluations");
	exit(-1);
}
print(scalar(@numEvaluations)."\n");
print EVALS (scalar(@numEvaluations)."\n");
print "$_\n" foreach(@numEvaluations);
print EVALS "$_\n" foreach(@numEvaluations);
close(EVALS);
`gzip $name`;
