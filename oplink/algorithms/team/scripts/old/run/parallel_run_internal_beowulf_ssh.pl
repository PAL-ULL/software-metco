#!/usr/bin/perl 
use threads;
use Thread::Queue;
use Cwd;
use File::Basename;
use Time::HiRes qw/usleep/;

my $dir = getcwd;
my $machineQueue, $taskQueue;

sub threadCode {
	my $task = shift;
	my $machine = shift;
	chomp($machine);
	$cmd = "ssh $machine $task";
	printf("Ejecuta: $cmd");
	`$cmd`;
	$machineQueue->enqueue($machine);
	$finish->enqueue(1);
}

#main
my $argc = @ARGV;

($argc == 1) || die("Uso: $0 fich_exp ");

$machineQueue = Thread::Queue->new;
$taskQueue = Thread::Queue->new;
$finish = Thread::Queue->new;
open CONFIGFILE, $ARGV[0];
while($line = <CONFIGFILE>){
	if ($line =~ m/Machinefile:([ \t]*)([a-zA-Z0-9\/_.]*)/i){
		$machineFileName = $2;
	} 
	if ($line =~ m/Name_of_experiment:([ \t]*)([a-zA-Z0-9_\/]*)/i){
		$name_of_experiment = $2;
	}
	if ($line =~ m/Preaction:(.*)/i){
		`$1`;
	}
}

defined("$name_of_experiment") || die("Experiment name non defined");
defined("$machineFileName") || die("Machinefile non defined");

open MACHINEFILE, $machineFileName;
$proc = 0;
while($nodo = <MACHINEFILE>){
	$proc++;
	$machineQueue->enqueue($nodo);
}

open EXPERIMENTS, "experimentsTmpFiles/$name_of_experiment/experiment_list";
$tasks = 0;
while($tarea = <EXPERIMENTS>){
	$tasks++;
	$taskQueue->enqueue($tarea);
}

($proc != 0) || die("Error: machines non specified");
($tasks != 0) || die("Error: tasks non specified");

$restantes = $tasks;
my @arg_thread;
while($tarea = $taskQueue->dequeue_nb()){
	sleep(1);
	#get the number of specified processors
	@arg_thread	= ();
  #usleep(100000);
	push(@arg_thread, $tarea);
	push(@arg_thread, $machineQueue->dequeue); 
	$thr = threads->new(\&threadCode, @arg_thread);
	$thr->detach;
	undef $thr;#bug de threads::perl. Si no, no libera la memoria de la thread cuando termine
	while(defined($finish->dequeue_nb)){
		$restantes--;
	}
}

#esperamos a que todo haya terminado
($finish->dequeue()) foreach (1..$restantes);
print ("*\n");
sleep 1;
exit;
