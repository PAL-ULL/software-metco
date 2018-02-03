#!/usr/bin/perl
use warnings;
use strict;

die "Usage: ./calculate_time.pl individualsFileName instanceFileName (./calculate_time.pl individuals/berlin_individuals.txt instanceFiles/berlin.txt)" if (@ARGV != 2);

my ($fileName, $instance) = @ARGV;
$instance =~ m/\/([a-z A-Z 0-9]+).txt/;
my $instanceName = $1;

open my $fileIn, "<", $fileName || die "Can't open file $fileName";

my ($sumTotalTime, $sumSumoTime) = (0, 0);

foreach my $line (<$fileIn>) {
  chomp $line;

  open my $fileOut, ">", "tl_${instanceName}.txt" || die "Can't open file tl_${instanceName}.txt";
  print $fileOut $line;
  my $result = `./code/sumo-wrapper $instance . tl_${instanceName}.txt result_${instanceName}.txt 1`;
  close $fileOut;

  $result =~ m/Total time:\s*(\d+)/;
  $sumTotalTime += $1;

  $result =~ m/SUMO time:\s*(\d+)/;
  $sumSumoTime += $1;
}

close $fileIn;

print "Total time: $sumTotalTime\n";
print "Sumo time: $sumSumoTime\n";
