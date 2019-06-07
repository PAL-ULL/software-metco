#!/usr/bin/perl 
use threads;
use Thread::Queue;
use Cwd;
use File::Basename;

my $dir = getcwd;
my $machineQueue, $taskQueue;

sub threadCode {
	my $task = shift;
	my @machines;
  if ($tarea =~ m/-machinefile([ \t]*)([a-zA-Z0-9\/_.]*)/){
		$machinefileThread = $2;
	} else {
		printf("Error: machinefile non specified in task: $tarea\n");
		exit(-1);
	}
	@machines = ();
	if (!(open MAC_FILE, ">$machinefileThread")){
		die("Could not create $machinefileThread");
	}

	while ($machine = shift){
		printf MAC_FILE $machine;
		push (@machines, $machine);
	}
	close MAC_FILE;
	printf("Ejecuta: $task\n");
	`$task`;
	$machineQueue->enqueue($_) foreach(@machines);
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
	if ($line =~ m/Name_of_experiment:([ \t]*)([a-zA-Z0-9\/_.]*)/i){
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
	if ($tarea =~ m/-np([ \t]+)(([0-9])+)/){
		$np = $2;
	} else {
		printf("Error: number of processors non specified in task: $tarea\n");
		exit(-1);
	}
	#get the number of specified processors
	@arg_thread	= ();
	push(@arg_thread, $tarea);
	push(@arg_thread, $machineQueue->dequeue) foreach(1..$np);
	sleep 1;
	$thr = threads->new(\&threadCode, @arg_thread);
	$thr->detach;
	undef $thr;#bug de threads::perl. Si no, no libera la memoria de la thread cuando termine

	while(defined($a = $finish->dequeue_nb)){
		$restantes--;
	}
}

#esperamos a que todo haya terminado
($finish->dequeue()) foreach (1..$restantes);
print("*\n");
sleep 1;#si no a veces da warning de que hay algun hijo vivo
exit;
