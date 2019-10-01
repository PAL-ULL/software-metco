#!/usr/bin/perl

my %info;
$maxValue = shift;
$filename = shift;
my $repetitions = 24;
my $period = 600;
my $lastEval = 41400;
$actualEval = $period;
@percent = ();
@actualValue = ();
@previousEval = ();
@previousValue = ();
@fin = ();
@valuesPercent = (1);
@values = ($maxValue);
$info{$filename} = {};
foreach(0..$repetitions){
	push(@percent, $valuesPercent[0]);
	push(@indexPercent, 0);
	push(@actualValue, $maxValue*$valuesPercent[0]);
	push(@previousEval, 0);
	push(@previousValue, 0);
	push(@fin, 0);
	%{$info{$filename}} = {};
}

my $metidos = 0;
$first = $filename;
while($actualEval <= $lastEval){
	open FILE, "${filename}$actualEval.allHV";
	$index = 0;
	while(<FILE>){
		$value = $_;
		if (!$fin[$index]){
			while ($value >= $actualValue[$index]){
				$m = ($value - $previousValue[$index])/($actualEval - $previousEval[$index]);
				$valorEval = ($actualValue[$index] - $previousValue[$index])/$m + $previousEval[$index];
				#push @{$info{$filename}{$actualValue[$index]}}, int($valorEval);#interpolando
				$metidos++;
				my $ratio = $metidos / $repetitions;
				printf("$actualEval $ratio\n");
				#printf("$actualEval\n");
				push @{$info{$filename}{$actualValue[$index]}}, $actualEval;#sin interpolar
				$indexPercent[$index]++;
				if ($indexPercent[$index] >= $#valuesPercent){
					$fin[$index] = 1;
					last;
				} else {
					$percent[$index] = $valuesPercent[$indexPercent[$index]];
				}
				$actualValue[$index] = $maxValue*$percent[$index];
			}
		}
		$previousEval[$index] = $actualEval;
		$previousValue[$index] = $value;
		$index++;
	}
	$actualEval += $period;
}

my $i = 0;
foreach(@values){
	$val = $_;
	$size = 0;
	#print "*****$val*****\n";
	$sum = 0;
	$j = 0;
	$index = 0;
	foreach(@{@{$info{$filename}}{$val}}){
		$size++;
		$value = $_;
		#print "$value\n";
		if ($index == 14){
			$value14 = $value;
		}
		if ($index == 15){
			$value15 = $value;
		}
		if ( ($index == 14) || ($index == 15)){
			printf("Pos $index: $value\n");
		}
		$index++;
		$sum += $value;
		$j++;
	}
	printf("Mediana: ". (($value14)+($value15))/2 . "\n");
	#print "Media: ".$sum/$j."\n";
	#print "Llegaron: $size\n";
	$i++;
}
