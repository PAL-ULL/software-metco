CC=g++ 
BISON=bison
FLEX=flex

all: parseExperiment

parseExperiment: parseSeqExp.tab.c lex.expSeq.c parseLine.tab.c lex.Line.c parseExperiment.C parseDefinitions.o parseSeqDefinitions.o parseLineDefinitions.o coverExperiments.o parseParDefinitions.o parseParExp.tab.c lex.expPar.c
	$(CC) $(IFLAGS) parseExperiment.C lex.expSeq.c parseLine.tab.c lex.Line.c parseSeqExp.tab.c parseDefinitions.o parseSeqDefinitions.o parseLineDefinitions.o coverExperiments.o parseParDefinitions.o parseParExp.tab.c lex.expPar.c -o $@

parseSeqExp.tab.c: parseSeqExp.y
	$(BISON) -p expSeq -d parseSeqExp.y

lex.expSeq.c: parseSeqExp.l
	$(FLEX) -PexpSeq -i parseSeqExp.l

parseLine.tab.c: parseLine.y
	$(BISON) -p Line -d parseLine.y 

lex.Line.c: parseLine.l
	$(FLEX) -PLine -i parseLine.l 

parseParExp.tab.c: parseParExp.y
	$(BISON) -p expPar -d parseParExp.y 

lex.expPar.c: parseParExp.l
	$(FLEX) -PexpPar -i parseParExp.l 

coverExperiments.o: coverExperiments.h coverExperiments.C
	$(CC) $(IFLAGS) -c coverExperiments.C -o $@

parseLineDefinitions.o: parseLineDefinitions.h parseLineDefinitions.C
	$(CC) -c parseLineDefinitions.C -o $@

parseDefinitions.o: parseDefinitions.h parseDefinitions.C
	$(CC) -c parseDefinitions.C -o $@

parseParDefinitions.o: parseParDefinitions.h parseParDefinitions.C
	$(CC) -c parseParDefinitions.C -o $@

parseSeqDefinitions.o: parseSeqDefinitions.h parseSeqDefinitions.C
	$(CC) -c parseSeqDefinitions.C -o $@

clean:
	rm *.o lex.expPar.c parseParExp.tab.c parseParExp.tab.h lex.Line.c parseLine.tab.c parseLine.tab.h parseSeqExp.tab.h parseSeqExp.tab.c lex.expSeq.c parseExperiment
