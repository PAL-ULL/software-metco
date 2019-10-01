#!/usr/bin/perl
use warnings;
use strict;
use Data::Dumper;

sub getStats {
  my $algorithm = shift;
  my $statistics = {};

  foreach my $fileName (`ls *${algorithm}*.allHV`) {
    chomp($fileName);

    my $scriptR = "data1 = scan(\\\"$fileName\\\"); summary(data1)\n";
    my @result = `echo "$scriptR" | R --silent --no-save`;

    chomp($result[2]);
    my $data = [ split /\s+/, $result[2] ];
    shift @$data;

    $statistics->{$fileName} = $data;
  }

  return $statistics;
}

sub getRankings {
  my ($algorithm, $numberRuns) = @_;
  my @fileNames = `ls *${algorithm}*.allHV`;
  my $rankings = {};

  for (my $i = 0; $i < @fileNames; $i++) {
    for (my $j = $i + 1; $j < @fileNames; $j++) {
      my $file_A = $fileNames[$i];
      my $file_B = $fileNames[$j];
      chomp $file_A;
      chomp $file_B;
      
      if (!defined($rankings->{$file_A})) {
          $rankings->{$file_A} = [0, 0, 0, 0];
      }
      if (!defined($rankings->{$file_B})) {
          $rankings->{$file_B} = [0, 0, 0, 0];
      }

      my $result = `./statisticalTests_old.pl $numberRuns $file_A $file_B`;

      if ($result =~ /Mejor: $file_A/) {
        $rankings->{$file_A}[0]++;
        $rankings->{$file_B}[1]++;
        $rankings->{$file_A}[3]++;
        $rankings->{$file_B}[3]--;
      }
      elsif ($result =~ /Mejor: $file_B/) {
        $rankings->{$file_B}[0]++;
        $rankings->{$file_A}[1]++;
        $rankings->{$file_B}[3]++;
        $rankings->{$file_A}[3]--;
      }
      elsif ($result =~ /No existe diferencia estadistica/) {
        $rankings->{$file_B}[2]++;
        $rankings->{$file_A}[2]++;
      }
      elsif ($result =~ /OJO/) {
        print "Warning: $file_A, $file_B";
      }
    }
  }
  return $rankings;
}

sub printHashSortedByValue {
  my ($algorithm, $statHash, $rankingHash, $field) = @_;

  foreach my $key (keys %$statHash) {
    push @{$statHash->{$key}}, @{$rankingHash->{$key}};
  }

  print "Configuration & Min. & 1st Qu. & Median & Mean & 3rd Qu. & Max. & W & L & Ranking \\\\ \\hline\n";
 
  foreach my $key (sort { $statHash->{$b}[$field] <=> $statHash->{$a}[$field] } keys %$statHash) {
    $key =~ /(${algorithm}.+)_days/;
    my $shortKey = $1;
    $shortKey =~ s/\_/\\\_/g;
    print "$shortKey ";
  
    for (my $i = 0; $i < 6; $i++) {
      print "& $statHash->{$key}[$i] ";
    }

    print "& $statHash->{$key}[6] & $statHash->{$key}[7] & $statHash->{$key}[9] \\\\\n";
  }
}

my $algorithm = shift || die "You must specify one of the following algorithms: NSGA2, SPEA2, IBEA";
my $numberRuns = shift || die "You must specify the number of runs you performed";

my $statistics = getStats $algorithm;
my $rankings = getRankings $algorithm, $numberRuns;

# Print configurations sorted by mean (field number 3)
# printHashSortedByValue($algorithm, $statistics, $rankings, 3);

# Print configurations sorted by ranking (field number 9)
printHashSortedByValue($algorithm, $statistics, $rankings, 9);
