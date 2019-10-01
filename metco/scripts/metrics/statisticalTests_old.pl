#!/usr/bin/perl

sub getMean {
	$file = shift;
	$scriptR = "data1 = scan(\\\"$file\\\");
							mean(data1)";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en getMean");
		exit(-1);
	}
}

sub getMedian {
	$file = shift;
	$scriptR = "data1 = scan(\\\"$file\\\");
							median(data1)\n";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en getMedian: $resultado");
		exit(-1);
	}
}

sub getSD {
	$file = shift;
	$scriptR = "data1 = scan(\\\"$file\\\");
							sd(data1)";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en getSD");
		exit(-1);
	}
}

sub kruskalWallisTest {
	#printf("Aplica kruskal wallis");	
	$file1 = shift;
	$file2 = shift;
	$scriptR = "data1 = scan(\\\"$file1\\\");
							data2 = scan(\\\"$file2\\\");
							require(\\\"lawstat\\\");
							";
	$scriptR .= "grupos<-c(";
	for my $i (1..$repeticiones){
		$scriptR .= "1, ";		
	}
	for my $i (1..$repeticiones-1){
		$scriptR .= "2, ";
	}
	$scriptR .= "2);\n";
	$scriptR .= "kruskal.test(c(data1, data2), grupos)\\\$p.value";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en kruskalWallisTest");
		exit(-1);
	}
}

sub welchTest{
	#printf("Aplica Welch");
	$file1 = shift;
	$file2 = shift;
	$scriptR = "data1 = scan(\\\"$file1\\\");
							data2 = scan(\\\"$file2\\\");
							t.test(data1, data2)\\\$p.value";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en welchTest");
		exit(-1);
	}
}

sub shapiroTest{
	#printf("Aplica Shapiro");
	$file = shift;
	#Test de normalidad -> Shapiro
	$scriptR = "data1 = scan(\\\"$file\\\");
							shapiro.test(data1)\\\$p.value";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en shapiro\n");
		exit(-1);
	}
}

sub leveneTest{
	#printf("Aplica Levenne");
	$file1 = shift;
	$file2 = shift;
	$scriptR = "data1 = scan(\\\"$file1\\\");
							data2 = scan(\\\"$file2\\\");
							require(\\\"lawstat\\\");
							";
	$scriptR .= "grupos<-c(";
	for my $i (1..$repeticiones){
		$scriptR .= "1, ";		
	}
	for my $i (1..$repeticiones-1){
		$scriptR .= "2, ";
	}
	$scriptR .= "2);\n";
	$scriptR .= "levene.test(c(data1, data2), grupos)\\\$p.value";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)/){
		return $1;
	} else {
		printf("Error interno. Fallo en levene");
		exit(-1);
	}
}

sub anovaTest{
	#printf("Aplica Anova");
	$scriptR = "data1 = scan(\\\"$file1\\\");
							data2 = scan(\\\"$file2\\\");";
	$scriptR .= "grupos<-c(";
	for my $i (1..$repeticiones){
		$scriptR .= "1, ";		
	}
	for my $i (1..$repeticiones-1){
		$scriptR .= "2, ";
	}
	$scriptR .= "2);\n";
	$scriptR .= "summary(aov(c(data1,data2)~grupos))[[1]][[\\\"Pr(>F)\\\"]]";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "\\\[1\\\]"`;
	if ($resultado =~ /\[1\] (.*)NA.*/){
		return $1;
	} else {
		printf("Error interno. Fallo en anova");
		exit(-1);
	}
}

sub VarghaDelaneyTest {
	$file1 = shift;
	$file2 = shift;
	$scriptR = "library(\"effsize\");
							data1 = scan(\\\"$file1\\\");
							data2 = scan(\\\"$file2\\\");
							VD.A(data1, data2);";
	$resultado = `echo "$scriptR" | R --silent --no-save | grep "estimate"`;
	if ($resultado =~ /A estimate: (\d+\.?\d*)/) {
		return $1;
	} else {
		printf("Error interno. Fallo en VarghaDelaneyTest");
		exit(-1);
	}
}

if (@ARGV != 3){
	printf("Error. Uso: repeticiones file1 file2\n");
	exit(-1);
	$file1 = shift;
	$file2 = shift;
}

$repeticiones = shift;
my $file1 = shift;
my $file2 = shift;
my $testName = "No-test";

# Hacemos el test de shapiro para comprobar la normalidad de las poblaciones
my $pvalueNormal1 = shapiroTest($file1);
my $pvalueNormal2 = shapiroTest($file2);

if (($pvalueNormal1 >= 0.05) && ($pvalueNormal2 >= 0.05)){ #Si ambos cumplen el test de normalidad, se tiene que aplicar levene para analizar las varianzas
	#Levene
	my $pvalueLevene = leveneTest($file1, $file2);
	if ($pvalueLevene >= 0.05) { #Varianzas iguales --> anova
		$pValue = anovaTest($file1, $file2);
    $testName = "ANOVA";
	} else {#Varianzas diferentes --> Welch
		$pValue = welchTest($file1, $file2);
    $testName = "Welch";
	}
} else { #No sigue una distribucion normal -> kruskal wallis
	$pValue = kruskalWallisTest($file1, $file2);
  $testName = "Kruskal-Wallis";
}

$effSize = VarghaDelaneyTest($file1, $file2);
$mean1 = getMean($file1);
$mean2 = getMean($file2);
$median1 = getMedian($file1);
$median2 = getMedian($file2);
$sd1 = getSD($file1);
$sd2 = getSD($file2);

printf("$mean1 $mean2 $median1 $median2 $sd1 $sd2 $pValue $testName $effSize\n");

if ($pValue < 0.05){ #OJO SUPONEMOS MINIMIZACION
  if (($mean1 > $mean2) && ($median1 < $median2)){
    printf("OJO\n");
  } elsif (($mean1 < $mean2) && ($median1 > $median2)){
    printf("OJO\n");
  } elsif ($median1 < $median2){
    printf("Mejor: $file1\n");
  } else {
    printf("Mejor: $file2\n");
  }
}
else {
  printf("No existe diferencia estadistica\n");
}
