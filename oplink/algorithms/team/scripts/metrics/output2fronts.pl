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
	$line = $_;
	if ($_ =~m/Front Size = (\d*)/i){
		$tam = $1;
		if ($anterior =~m/Current Evaluation = (\d*)/i){
			$lastEval = $1;
		} else {
			$lastEval = $anterior + 0;
		}
		push(@numEvaluations, $lastEval);
		if (!open FRENTE, ">${name}_${lastEval}.front"  ) { 
			printf("Error creating file ${name}_${lastEval}.front");
			exit(-1);
		} 
		$abierto = 1;

		if ($abierto == 1){
			foreach (1..$tam){
				$line = <>;
				print FRENTE "$line";
			}
			close (FRENTE);
			$abierto = 0;
		}
	}
	$anterior = $line;
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
