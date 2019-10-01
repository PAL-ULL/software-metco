CC=g++ -Wall
BISON=bison
FLEX=flex
IFLAGS= -I../run/ -I../../src/skeleton -I../../src/skeleton/moea/ -I../../src/skeleton/utils/ -I../../src_external/hv-1.1-src/
CCFLAGS = -c -O3

all: analyzeExperiments generationalDistanceReferenceFront mergeFrontsExperiments calculateContributions calculateCoverages calculateEpsilonInd

analyzeExperiments: parseParExp.tab.c lex.expPar.c analyzeExperiments.C MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o parseConf.tab.c lex.Conf.c utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o lex.metricFile.c parseMetric.tab.c parseSeqExp.tab.c lex.expSeq.c metricDefinitions.o parseLineDefinitions.o parseLine.tab.c lex.Line.c getIndividuals.o coverExperiments.o EA.o Conversion.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o hv.o avl.o MultiObjectivization.o
	$(CC) $(IFLAGS) analyzeExperiments.C parseParExp.tab.c lex.expPar.c parseConf.tab.c lex.Conf.c MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o lex.metricFile.c parseMetric.tab.c parseSeqExp.tab.c lex.expSeq.c metricDefinitions.o parseLineDefinitions.o parseLine.tab.c lex.Line.c getIndividuals.o coverExperiments.o EA.o Conversion.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o hv.o avl.o MultiObjectivization.o -o $@ -ldl

metricDefinitions.o: metricDefinitions.C metricDefinitions.h
	$(CC) -c -o $@ metricDefinitions.C

lex.metricFile.c: parseMetric.l
	$(FLEX) -PmetricFile -i parseMetric.l 

parseMetric.tab.c: parseMetric.y 
	$(BISON) -p metricFile -d parseMetric.y 

generationalDistanceReferenceFront: generationalDistanceReferenceFront.C Individual.o Plugin.o MOFront.o MOFrontVector.o MOFrontBinaryInteger.o utils.o hv.o avl.o
	$(CC) $(IFLAGS) generationalDistanceReferenceFront.C Individual.o Plugin.o MOFront.o MOFrontVector.o MOFrontBinaryInteger.o utils.o hv.o avl.o -o $@ -ldl

calculateEpsilonInd: calculateEpsilonInd.C Individual.o Plugin.o MOFront.o MOFrontVector.o MOFrontBinaryInteger.o utils.o MinimizationGenericIndividual.o hv.o avl.o
	$(CC) $(IFLAGS) calculateEpsilonInd.C Individual.o Plugin.o MOFront.o MOFrontVector.o MOFrontBinaryInteger.o utils.o MinimizationGenericIndividual.o hv.o avl.o -o $@ -ldl

Individual.o: ../../src/skeleton/Individual.cpp ../../src/skeleton/Individual.h
	$(CC) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/Individual.cpp -o $@

MinimizationGenericIndividual.o: MinimizationGenericIndividual.cpp MinimizationGenericIndividual.h ../../src/skeleton/Individual.h
	$(CC) $(IFLAGS) $(CCFLAGS) MinimizationGenericIndividual.cpp -o $@

mergeFrontsExperiments: parseParExp.tab.c lex.expPar.c mergeFrontsExperiments.C MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o parseConf.tab.c lex.Conf.c utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o Conversion.o hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o
	$(CC) $(IFLAGS) mergeFrontsExperiments.C parseParExp.tab.c lex.expPar.c parseConf.tab.c lex.Conf.c MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o -o $@ -ldl Conversion.o hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o

calculateContributions: parseParExp.tab.c lex.expPar.c calculateContributions.C MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o parseConf.tab.c lex.Conf.c utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o Conversion.o hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o
	$(CC) $(IFLAGS) calculateContributions.C parseParExp.tab.c lex.expPar.c parseConf.tab.c lex.Conf.c MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o Conversion.o -o $@ -ldl hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o 

calculateCoverages: parseParExp.tab.c lex.expPar.c calculateCoverages.C MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o parseConf.tab.c lex.Conf.c utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o Conversion.o hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o 
	$(CC) $(IFLAGS) calculateCoverages.C parseParExp.tab.c lex.expPar.c parseConf.tab.c lex.Conf.c MOFront.o MOFrontVector.o MOFrontBinaryInteger.o Individual.o Plugin.o Configuration.o utils.o parseDefinitions.o parseParDefinitions.o parseSeqDefinitions.o parseSeqExp.tab.c lex.expSeq.c getIndividuals.o parseLineDefinitions.o parseLine.tab.c lex.Line.c coverExperiments.o EA.o Conversion.o -o $@ -ldl hv.o avl.o Mutation.o Crossover.o InitPopIslandLoader.o MigrationTopology.o LocalSearch.o ScoreAlgorithm.o GlobalScoreAlgorithm.o LocalScoreAlgorithm.o MigrationSelector.o ExchangeSelector.o MultiObjectivization.o 

parseParExp.tab.c: ../run/parseParExp.y 
	$(BISON) -p expPar -d ../run/parseParExp.y 

lex.expPar.c: ../run/parseParExp.l
	$(FLEX) -PexpPar -i ../run/parseParExp.l 

parseSeqExp.tab.c: ../run/parseSeqExp.y
	$(BISON) -p expSeq -d ../run/parseSeqExp.y

lex.expSeq.c: ../run/parseSeqExp.l
	$(FLEX) -PexpSeq -i ../run/parseSeqExp.l

MOFront.o: ../../src/skeleton/MOFront.cpp ../../src/skeleton/MOFront.h ../../src/skeleton/Definitions.h 
	$(CC) -DNO_MPI -DVARIANT=4 $(CCFLAGS) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/MOFront.cpp -o $@

MOFrontBinaryInteger.o: ../../src/skeleton/MOFrontBinaryInteger.cpp ../../src/skeleton/MOFrontBinaryInteger.h ../../src/skeleton/Definitions.h 
	$(CC) -DNO_MPI -DVARIANT=4 $(CCFLAGS) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/MOFrontBinaryInteger.cpp -o $@

MOFrontVector.o: ../../src/skeleton/MOFrontVector.cpp ../../src/skeleton/MOFrontVector.h ../../src/skeleton/Definitions.h 
	$(CC) -DNO_MPI -DVARIANT=4 $(CCFLAGS) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/MOFrontVector.cpp -o $@


parseConf.tab.c: ../../src/skeleton/parseConf.y
	$(BISON) -p Conf -d ../../src/skeleton/parseConf.y 

lex.Conf.c: ../../src/skeleton/parseConf.l
	$(FLEX) -PConf -i ../../src/skeleton/parseConf.l 

getIndividuals.o: getIndividuals.h getIndividuals.C
	$(CC) $(IFLAGS) $(CCFLAGS) getIndividuals.C -o $@

Configuration.o: ../../src/skeleton/Configuration.cpp ../../src/skeleton/Configuration.h
	$(CC) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/Configuration.cpp -o $@

parseDefinitions.o: ../run/parseDefinitions.h ../run/parseDefinitions.C
	$(CC) -c ../run/parseDefinitions.C -o $@

parseParDefinitions.o: ../run/parseParDefinitions.h ../run/parseParDefinitions.C
	$(CC) -c ../run/parseParDefinitions.C -o $@

parseSeqDefinitions.o: ../run/parseSeqDefinitions.h ../run/parseSeqDefinitions.C
	$(CC) -c ../run/parseSeqDefinitions.C -o $@

parseLineDefinitions.o: ../run/parseLineDefinitions.h ../run/parseLineDefinitions.C
	$(CC) -c ../run/parseLineDefinitions.C -o $@

coverExperiments.o: ../run/coverExperiments.h ../run/coverExperiments.C
	$(CC) -c ../run/coverExperiments.C -o $@

parseLine.tab.c: ../run/parseLine.y
	$(BISON) -p Line -d ../run/parseLine.y 

lex.Line.c: ../run/parseLine.l
	$(FLEX) -PLine -i ../run/parseLine.l 

utils.o: ../../src/skeleton/utils/utils.h ../../src/skeleton/utils/utils.cpp 
	$(CC) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/utils/utils.cpp -o $@ 

EA.o: ../../src/skeleton/EA.h ../../src/skeleton/EA.cpp ../../src/skeleton/Individual.h ../../src/skeleton/MOFront.h ../../src/skeleton/Configuration.h ../../src/skeleton/Definitions.h ../../src/skeleton/utils/utils.h
	$(CC) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/EA.cpp -o $@

Conversion.o: ../../src/skeleton/Conversion.cpp ../../src/skeleton/Individual.h ../../src/skeleton/Definitions.h ../../src/skeleton/Conversion.h 
	$(CC) $(IFLAGS) $(CCFLAGS) ../../src/skeleton/Conversion.cpp -o $@ 

Plugin.o: ../../src/skeleton/Plugin.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

Mutation.o: ../../src/skeleton/Mutation.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

MultiObjectivization.o: ../../src/skeleton/MultiObjectivization.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@


Crossover.o: ../../src/skeleton/Crossover.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

InitPopIslandLoader.o: ../../src/skeleton/InitPopIslandLoader.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

MigrationTopology.o: ../../src/skeleton/MigrationTopology.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

LocalSearch.o: ../../src/skeleton/LocalSearch.cpp
	$(CXX) $(IFLAGS) $(CCFLAGS) $< -o $@

ScoreAlgorithm.o: ../../src/skeleton/ScoreAlgorithm.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

GlobalScoreAlgorithm.o: ../../src/skeleton/GlobalScoreAlgorithm.cpp 
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

LocalScoreAlgorithm.o: ../../src/skeleton/LocalScoreAlgorithm.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

MigrationSelector.o: ../../src/skeleton/MigrationSelector.cpp
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

ExchangeSelector.o: ../../src/skeleton/ExchangeSelector.cpp 
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

hv.o: ../../src_external/hv-1.1-src/hv.c
	$(CC) $(IFLAGS) $(CCFLAGS) -DVARIANT=4 $< -o $@

avl.o: ../../src_external/hv-1.1-src/avl.c
	$(CC) $(IFLAGS) $(CCFLAGS) $< -o $@

clean:
	rm *.o analyzeExperiments mergeFrontsExperiments calculateContributions calculateEpsilonInd calculateCoverages parseConf.tab.h lex.Conf.c lex.expPar.c parseParExp.tab.c parseParExp.tab.h lexConf.c parseConf.tab.c generationalDistanceReferenceFront lex.expSeq.c parseSeqExp.tab.c parseSeqExp.tab.h lex.metricFile.c parseMetric.tab.c parseMetric.tab.h parseLine.tab.c parseLine.tab.h lex.Line.c
