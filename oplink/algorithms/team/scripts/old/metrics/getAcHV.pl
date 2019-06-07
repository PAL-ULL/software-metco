#!/usr/bin/perl

sub analyze{
	my $repetitions = 30;
	my $period = 900;
	my $lastEval = 90000;
	my $actualEval = $period;
	my $filename = shift;
	my %info;
	my @percent = ();
	my @indexPercent = ();
	my @actualValue = ();
	my @previousEval = ();
	my @previousValue = ();
	my @fin = ();
	my @valuesPercent = (0.80, 0.85, 0.90, 0.95, 0.99);
	my @values = ($valuesPercent[0]*$maxValue, $valuesPercent[1]*$maxValue, $valuesPercent[2]*$maxValue, $valuesPercent[3]*$maxValue, $valuesPercent[4]*$maxValue);
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

	my $first = $filename;
	while($actualEval <= $lastEval){
		open FILE, "${filename}$actualEval.allHV";
		my $index = 0;
		while(<FILE>){
			my $value = $_;
			if (!$fin[$index]){
				while ($value > $actualValue[$index]){
					my $m = ($value - $previousValue[$index])/($actualEval - $previousEval[$index]);
					my $valorEval = ($actualValue[$index] - $previousValue[$index])/$m + $previousEval[$index];
					push @{$info{$filename}{$actualValue[$index]}}, int($valorEval);#interpolando
					#push @{$info{$filename}{$actualValue[$index]}}, $actualEval;#sin interpolar
					$indexPercent[$index]++;
					if ($indexPercent[$index] > $#valuesPercent){
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
		close FILE;
		$actualEval += $period;
	}

	my $i = 0;
	my @media = ();
	foreach(@values){
		my $val = $_;
		$sum = 0;
		$j = 0;
		$index = 0;
		foreach(@{@{$info{$filename}}{$val}}){
			$value = $_;
			$index++;
			$sum += $value;
			$j++;
		}
		push @media, ($sum/$j);
	}
	return @media;
}

$maxValue = shift;
$filename1 = shift;
$nproc1 = shift;
$filename2 = shift;
$nproc2 = shift;
@evals1 = analyze($filename1);
@evals2 = analyze($filename2);

foreach(0..scalar(@evals1)-1){
	print 1.0*$nproc1/$nproc2*$evals2[$_]/$evals1[$_]."\n";
}
