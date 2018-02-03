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

while(<>){
	if ($_ =~m/Current Evaluation = (\d*)/i){
		`rm ${name}_$1.front`
	}
}
`gzip $name`;
