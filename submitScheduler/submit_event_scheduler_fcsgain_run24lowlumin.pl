#!/usr/bin/perl
use strict;
use warnings;

my $current_folder=$ENV{'PWD'};
chomp($current_folder);

my $folder = substr $current_folder, 0,-15;
print "working folder: $folder\n";

#print "Please write down the entire run number or the first five digit of the run number:\n";
#my $run =<STDIN>;
#chomp($run);
#my $day = substr($run,2,3);
#my $yy = substr($run,0,2);
#my $yy22 = "22";
#my $yy23 = "23";
#my $yy24 = "24";
#my $year = "";
#chomp($yy);
#if ($yy eq $yy22) {print "$yy \n"; 
#			$year = "2021";}
#if ($yy eq $yy23) {print "$yy \n";
#			$year = "2022";}
#if ($yy eq $yy24) {print "$yy \n";
#			$year = "2023";}
#print "year = $year \n";
#my $mudstroot = "";
#if ($yy eq $yy23) {$mudstroot="/star/data1*/reco/production_pp500_2022/ReversedFullField/pp500_22_DEV_fcs/$year/$day/$run*/st_fwd*.MuDst.root";}
#if ($yy eq $yy24) {$mudstroot="/star/data1*/reco/production_AuAu_2023/ReversedFullField/P24ia_calib/$year/$day/$run*/st_fcs*MuDst.root";}
#print "$mudstroot \n";
#
#my $pi0root = "";
#if ($yy eq $yy23)
#{
#	$pi0root="$folder/run22root/$run/";
#	if (not -d $pi0root) {mkdir $pi0root or die "can not mkdir $pi0root \n";}
#}
#
#if ($yy eq $yy24)
#{
#	$pi0root="$folder/run23root/$run/";
#	if (not -d $pi0root) {mkdir $pi0root or die "can not mkdir $pi0root \n";}
#}

my $pi0root = "$folder/run24root/run24lowlumin/";
if (not -d $pi0root) {mkdir $pi0root or die "can not mkdir $pi0root \n";}

my $log="$folder/submitScheduler/log/";
if (not -d $log) {mkdir $log or die "can not mkdir $log \n";}

my $outroot = "$pi0root/StFcsPi0invariantmass_run24lowlumin_*.root";
print "Remove all files in test folder $outroot (y/n):\n";
my $input = <STDIN>; chomp $input;
if( $input eq "y" ){system("/bin/rm $outroot") == 0 or "Unable to remove files in '$outroot': $! Directories are either empty or don't exist.";}

#my $filelist="$folder/submitScheduler/$run"."eventroot.list";
#if (-f $filelist) {system("rm $filelist");}
#system("ls $mudstroot > $filelist");


system("star-submit-template -template submitScheduler_dataevent_run24_lowlumin.xml -entities folder=$folder\n")==0 or die "sorry, unable to submit the jobs";
#if ($yy eq $yy23)
#{
#system("star-submit-template -template submitScheduler_dataevent_run22.xml -entities folder=$folder,runnumber=$run\n")==0 or die "sorry, unable to submit the jobs";
#}
#if ($yy eq $yy24)
#{
##system("star-submit-template -template submitScheduler_dataevent_run23.xml -entities folder=$folder,runnumber=$run\n")==0 or die "sorry, unable to submit the jobs";
#}

