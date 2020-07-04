
if ExistsVMPI
MPICC=${mpi}
parallel = metcoPar
else
if Exists2VMPI
MPICC=${mpi}
parallel = metcoPar
else
MPICC=mpiCC
parallel = metcoPar
endif
endif

if condPlugins
temp=${plugin_path}
else
temp=${HOME}/metco
endif
includedir = ${temp}


bin_PROGRAMS= metcoSeq $(parallel)
nodist_include_HEADERS = *.so
dist-hook:
	mkdir -p $(distdir)/src/skeleton
	mkdir -p $(distdir)/src/skeleton/main
	mv $(distdir)/*.cpp $(distdir)/src/skeleton/main
	mv $(distdir)/configure* $(distdir)/src/skeleton/main
	mv $(distdir)/Makefile* $(distdir)/src/skeleton/main
	mv $(distdir)/aclocal.m4 $(distdir)/src/skeleton/main
	make -f makefile_copyplugins DIST_DIR=$(distdir)
	cp ../*.cpp $(distdir)/src/skeleton
	cp ../*.h $(distdir)/src/skeleton
	cp ../*.c $(distdir)/src/skeleton
	cp ../*.y $(distdir)/src/skeleton
	cp ../*.l $(distdir)/src/skeleton
	cp ./makefile_* $(distdir)/src/skeleton/main
	mkdir -p $(distdir)/src/skeleton/utils/
	cp ../utils/*.h ../utils/*.cpp $(distdir)/src/skeleton/utils/
	cp -R ../../../src_external $(distdir)/


bindir = ${prefix}



#ACLOCAL_AMFLAGS = -I configureFiles

metcoSeq_SOURCES = metcoSeq.cpp
metcoPar_SOURCES = metcoPar.cpp
##main_SOURCES =

FLEX = flex
BISON = bison
CCFLAGS = -c -O3 -std=c++14
IFLAGS = -I. -I.. -I../utils -I../../../src_external/hv-1.1-src
DFLAGS =
LFLAGS = -fPIC

*.so:
../../problems/*:
all: sequential ${parallel} problems algorithms conversions scores migrationSelectors exchangeSelectors mutations crossovers migrationTopologies initPopIslandLoaders localSearches outputPrinters multiObjectivizations decompositions
sequential: metcoSeq
parallel: metcoPar
problems:
	make -s -f makefile_problems CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
algorithms:
	make -s -f makefile_algorithms CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
conversions:
	make -s -f makefile_conversions CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
scores:
	make -s -f makefile_scores CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
migrationSelectors:
	make -s -f makefile_migration_selectors CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
exchangeSelectors:
	make -s -f makefile_exchange_selectors CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
mutations:
	make -s -f makefile_mutations CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
localSearches:
	make -s -f makefile_localSearches CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
crossovers:
	make -s -f makefile_crossovers CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
migrationTopologies:
	make -s -f makefile_migration_topologies CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
initPopIslandLoaders:
	make -s -f makefile_init_pop_island_loaders CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
outputPrinters:
	make -s -f makefile_output_printers CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
multiObjectivizations:
	make -s -f makefile_multiobjectivizations CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"
decompositions:
		make -s -f makefile_decompositions CXX="$(CXX)" CCFLAGS="$(CCFLAGS)"


######################## sequential #####################################

metcoSeq$(EXEEXT): metcoSeq.cpp Individual.o MOFrontBinaryInteger.o MOFrontVector_noMPI.o MOFront.o hv.o avl.o EA.o utils.o ScoreAlgorithm.o Mutation.o Crossover.o LocalSearch.o Plugin.o OutputPrinter.o MultiObjectivization.o Decomposition.o
	$(CXX) -DNO_CONF_PARSING $(IFLAGS) metcoSeq.cpp Individual.o MOFrontBinaryInteger.o MOFrontVector_noMPI.o MOFront.o hv.o avl.o EA.o utils.o ScoreAlgorithm.o Mutation.o Crossover.o LocalSearch.o Plugin.o OutputPrinter.o MultiObjectivization.o Decomposition.o -o $@ -ldl



#########################################################################

############################# parallel ##################################

metcoPar$(EXEEXT): metcoPar.cpp MOFrontBinaryInteger.o MOFrontVector.o MOFront.o hv.o avl.o Individual.o EA.o ExecutionIsland.o CoordinatorIsland.o Configuration.o utils.o parseConf.tab.c lex.Conf.c Conversion.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o Mutation.o Crossover.o LocalSearch.o MigrationTopology.o InitPopIslandLoader.o Plugin.o MultiObjectivization.o Decomposition.o
	$(MPICC) $(IFLAGS) metcoPar.cpp Individual.o MOFrontBinaryInteger.o MOFrontVector.o MOFront.o hv.o avl.o EA.o ExecutionIsland.o utils.o CoordinatorIsland.o Configuration.o parseConf.tab.c lex.Conf.c Conversion.o ScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o Mutation.o Crossover.o LocalSearch.o MigrationTopology.o InitPopIslandLoader.o Plugin.o MultiObjectivization.o Decomposition.o -o $@ -ldl


#########################################################################

############################# skeleton ##################################

parseConf.tab.c: ../parseConf.y
	$(BISON) -p Conf -d ../parseConf.y

lex.Conf.c: ../parseConf.l
	$(FLEX) -PConf -i ../parseConf.l

Plugin.o: ../Plugin.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

CoordinatorIsland.o: ../CoordinatorIsland.cpp
	$(MPICC) $(IFLAGS) $(CCFLAGS) $< -o $@

Configuration.o: ../Configuration.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

utils.o: ../utils/utils.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

ExecutionIsland.o: ../ExecutionIsland.cpp
	$(MPICC) $(IFLAGS) $(CCFLAGS) $< -o $@

EA.o: ../EA.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

MOFront.o: ../MOFront.cpp
	$(MPICC) $(IFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

MOFrontVector.o: ../MOFrontVector.cpp
	$(MPICC) $(IFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

MOFrontBinaryInteger.o: ../MOFrontBinaryInteger.cpp
	$(MPICC) $(IFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

MOFrontVector_noMPI.o: ../MOFrontVector.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) -DNO_MPI -DVARIANT=4 $< -o $@

Individual.o: ../Individual.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

SimpleIndividual.o: ../SimpleIndividual.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

Conversion.o: ../Conversion.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

ScoreAlgorithm.o: ../ScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

GlobalScoreAlgorithm.o: ../GlobalScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

LocalScoreAlgorithm.o: ../LocalScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

MigrationSelector.o: ../MigrationSelector.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

ExchangeSelector.o: ../ExchangeSelector.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

Mutation.o: ../Mutation.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

Crossover.o: ../Crossover.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

MultiObjectivization.o: ../MultiObjectivization.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

Decomposition.o: ../Decomposition.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

LocalSearch.o: ../LocalSearch.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

MigrationTopology.o: ../MigrationTopology.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

OutputPrinter.o: ../OutputPrinter.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

InitPopIslandLoader.o: ../InitPopIslandLoader.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

EA.reloc.o: ../EA.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Individual.reloc.o: ../Individual.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

MOFrontVector_noMPI.reloc.o: ../MOFrontVector.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) -DNO_MPI -DVARIANT=4 $< -o $@

MOFront.reloc.o: ../MOFront.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

MOFrontBinaryInteger.reloc.o: ../MOFrontBinaryInteger.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

hv.reloc.o: ../../../src_external/hv-1.1-src/hv.c
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) -DVARIANT=4  $< -o $@

avl.reloc.o: ../../../src_external/hv-1.1-src/avl.c
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

utils.reloc.o: ../utils/utils.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Plugin.reloc.o: ../Plugin.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Conversion.reloc.o: ../Conversion.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Crossover.reloc.o: ../Crossover.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

MultiObjectivization.reloc.o: ../MultiObjectivization.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Decomposition.reloc.o: ../Decomposition.cpp
		$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

Mutation.reloc.o: ../Mutation.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

ExchangeSelector.reloc.o: ../ExchangeSelector.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

InitPopIslandLoader.reloc.o: ../InitPopIslandLoader.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

LocalSearch.reloc.o: ../LocalSearch.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

SimpleIndividual.reloc.o: ../SimpleIndividual.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

MigrationSelector.reloc.o: ../MigrationSelector.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

MigrationTopology.reloc.o: ../MigrationTopology.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

ScoreAlgorithm.reloc.o: ../ScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

GlobalScoreAlgorithm.reloc.o: ../GlobalScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

LocalScoreAlgorithm.reloc.o: ../LocalScoreAlgorithm.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@

OutputPrinter.reloc.o: ../OutputPrinter.cpp
	$(CXX) $(IFLAGS) $(LFLAGS) $(CCFLAGS) $< -o $@



#########################################################################

## Hipervolumen

hv.o: ../../../src_external/hv-1.1-src/hv.c
	$(CXX) $(IFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

avl.o: ../../../src_external/hv-1.1-src/avl.c
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

#########################################################################
clean:
	rm -f *.o *.so *.stackdump *.tmp *~ out metcoSeq metcoPar parseConf.tab.h parseConf.tab.c lex.Conf.c
	make -s -f makefile_algorithms clean
	make -s -f makefile_conversions clean
	make -s -f makefile_crossovers clean
	make -s -f makefile_exchange_selectors clean
	make -s -f makefile_migration_selectors clean
	make -s -f makefile_mutations clean
	make -s -f makefile_localSearches clean
	make -s -f makefile_problems clean
	make -s -f makefile_scores clean
	make -s -f makefile_output_printers clean
	make -s -f makefile_multiobjectivizations clean
	make -s -f makefile_decompositions clean

# Dependencias creadas usando makedepend
metcoPar.o: ../CoordinatorIsland.h ../Configuration.h ../MigrationSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h ../ExchangeSelector.h ../ScoreAlgorithm.h ../Conversion.h ../EA.h ../utils/utils.h ../MOFrontVector.h ../MOFrontBinaryInteger.h ../MOFront.h ../LocalScoreAlgorithm.h ../ExecutionIsland.h
metcoSeq.o: ../EA.h ../Plugin.h ../utils/utils.h ../Individual.h ../Definitions.h ../Mutation.h ../Crossover.h ../MOFrontVector.h ../MOFrontBinaryInteger.h ../MOFront.h ../LocalScoreAlgorithm.h ../ScoreAlgorithm.h
CoordinatorIsland.o: ../CoordinatorIsland.h ../Configuration.h ../MigrationSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h ../ExchangeSelector.h ../ScoreAlgorithm.h ../Conversion.h ../EA.h ../utils/utils.h ../MOFrontVector.h ../MOFrontBinaryInteger.h ../MOFront.h ../LocalScoreAlgorithm.h ../ExecutionIsland.h ../GlobalScoreAlgorithm.h
Configuration.o: ../Configuration.h ../MigrationSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h ../ExchangeSelector.h ../ScoreAlgorithm.h ../Conversion.h ../EA.h ../utils/utils.h ../MOFrontBinaryInteger.h ../MOFrontVector.h ../MOFront.h ../LocalScoreAlgorithm.h
ExecutionIsland.o: ../Configuration.h ../MigrationSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h ../ExchangeSelector.h ../ScoreAlgorithm.h ../Conversion.h ../EA.h ../utils/utils.h ../MOFrontBinaryInteger.h ../MOFrontVector.h ../MOFront.h ../LocalScoreAlgorithm.h ../ExecutionIsland.h ../CoordinatorIsland.h
EA.o: ../EA.h ../Plugin.h ../utils/utils.h ../Individual.h ../Definitions.h ../Mutation.h ../Crossover.h ../MOFrontBinaryInteger.h ../MOFrontVector.h ../MOFront.h ../LocalScoreAlgorithm.h ../ScoreAlgorithm.h ../Configuration.h ../MigrationSelector.h ../ExchangeSelector.h ../Conversion.h ../LocalSearch.h
MOFrontVector.o: ../MOFrontVector.h ../MOFront.h ../Definitions.h ../Individual.h ../Plugin.h ../Mutation.h ../Crossover.h ../../../src_external/hv-1.1-src/hv.h ../utils/utils.h
MOFrontBinaryInteger.o: ../MOFrontBinaryInteger.h ../MOFront.h ../Definitions.h ../Individual.h ../Plugin.h ../Mutation.h ../Crossover.h ../../../src_external/hv-1.1-src/hv.h ../utils/utils.h

MOFront.o: ../MOFront.h ../Definitions.h ../Individual.h ../Plugin.h ../Mutation.h ../Crossover.h ../../../src_external/hv-1.1-src/hv.h ../utils/utils.h
Individual.o: ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h
Conversion.o: ../Conversion.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h
ScoreAlgorithm.o: ../ScoreAlgorithm.h ../Plugin.h
GlobalScoreAlgorithm.o: ../GlobalScoreAlgorithm.h ../ScoreAlgorithm.h ../Plugin.h ../MOFront.h ../Definitions.h ../Individual.h ../Mutation.h ../Crossover.h
LocalScoreAlgorithm.o: ../LocalScoreAlgorithm.h ../ScoreAlgorithm.h ../Plugin.h ../MOFront.h ../Definitions.h ../Individual.h ../Mutation.h ../Crossover.h
MigrationSelector.o: ../MigrationSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h
ExchangeSelector.o: ../ExchangeSelector.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h
Mutation.o: ../Mutation.h ../Individual.h ../Plugin.h ../Definitions.h ../Crossover.h
Crossover.o: ../Crossover.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h
parseConf.tab.o: ../Conversion.h ../Individual.h ../Plugin.h ../Definitions.h ../Mutation.h ../Crossover.h ../ScoreAlgorithm.h ../MigrationSelector.h ../ExchangeSelector.h ../Configuration.h ../EA.h ../utils/utils.h ../MOFront.h ../MOFrontVector.h ../MOFrontBinaryInteger.h ../LocalScoreAlgorithm.h
hv.o: ../../../src_external/hv-1.1-src/hv.h ../../../src_external/hv-1.1-src/avl.h
avl.o: ../../../src_external/hv-1.1-src/avl.h
Plugin.o: ../Plugin.h
