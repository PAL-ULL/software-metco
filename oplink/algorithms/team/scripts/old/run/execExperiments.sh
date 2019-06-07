for i in `ls $1/*.pexp | xargs`; do
  ./execParallelExperiment.sh $i;
done;

for i in `ls $1/*.sexp | xargs`; do
	./execSeqExperiment.sh $i;
done;
