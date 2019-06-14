#!/usr/bin/perl

my %info;
$maxValue = shift;
my @usedProcs;
my $repetitions = 30;
my $period = 1500;
my $lastEval = 90000;
$filename = shift;
$actualEval = $period;
@percent = ();
@actualValue = ();
@previousEval = ();
@previousValue = ();
@fin = ();
@valuesPercent = (0.90, 0.95, 0.99);
@values = (2 - (2 -$maxValue)*$valuesPercent[0], 2 - (2 - $maxValue)*$valuesPercent[1], 2 - (2 - $maxValue)*$valuesPercent[2]);
$info{$filename} = {};
foreach(0..$repetitions){
	push(@percent, $valuesPercent[0]);
	push(@indexPercent, 0);
	push(@actualValue, 2 - (2 - $maxValue)*$valuesPercent[0]);
	push(@previousEval, 0);
	push(@previousValue, 2);
	push(@fin, 0);
	%{$info{$filename}} = {};
}

while($actualEval <= $lastEval){
	open FILE, "${filename}$actualEval.allEpsInd";
	$index = 0;
	while(<FILE>){
		$value = $_;
		if (!$fin[$index]){
			while ($value < $actualValue[$index]){
				$m = ($value - $previousValue[$index])/($actualEval - $previousEval[$index]);
				$valorEval = ($actualValue[$index] - $previousValue[$index])/$m + $previousEval[$index];
				push @{$info{$filename}{$actualValue[$index]}}, int($valorEval);#interpolando
				#push @{$info{$filename}{$actualValue[$index]}}, $actualEval;#sin interpolar
				$indexPercent[$index]++;
				if ($indexPercent[$index] > $#valuesPercent){
					$fin[$index] = 1;
					last;
				} else {
					$percent[$index] = $valuesPercent[$indexPercent[$index]];
				}
				$actualValue[$index] = 2 - (2 - $maxValue)*$percent[$index];
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
	print "*****$val*****\n";
	$sum = 0;
	$index = 0;
	$j = 0;
	foreach(@{@{$info{$filename}}{$val}}){
		$j++;
		$value = $_;
		print "$value\n";
		$index++;
		$sum += $value;
	}
	print "Media: ".$sum/$j."\n";
	$i++;
}
