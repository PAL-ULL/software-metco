#!/usr/bin/perl
use warnings;
use strict;
use File::Basename;

foreach my $config (`ls ../../experimentConfigFiles/Bolido_Sequential_MenuPlanning_EvoComp/*.sexp`) {

  my ($name,$path,$suffix) = fileparse($config, "sexp");
  $name =~ s/\s+//g;

  my $content = qq{ExperimentFiles: [ experimentConfigFiles/Bolido_Sequential_MenuPlanning_EvoComp/$name$suffix ]

GeneralFrontName:  experimentsResults/referenceFronts/Bolido_Sequential_MenuPlanning_EvoComp};


  open FILE, "> ${name}metric";
  print FILE $content;
  close FILE;

}
