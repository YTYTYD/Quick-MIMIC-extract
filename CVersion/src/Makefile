CC = mpicc
FLAG = -lm
INCL = -I ./include -I ./
SRC = main.c ./src/admissionsPRead.c ./src/consts.c ./src/data_trans.c ./src/drugExtract.c ./src/drugRead.c ./src/eventsRead.c ./src/featureExtract.c ./src/fextract.c ./src/globals.c ./src/icdCodes.c ./src/icustaysExtractPRead.c ./src/idExtract.c ./src/init.c ./src/noteExtract.c ./src/noteRead.c ./src/patientsPRead.c ./src/sInfoExtract.c ./src/taskInit.c ./src/taskrecv.c ./src/transfersPRead.c ./src/utils.c

all:extracter

debug:$(SRC)
	$(CC) $? -g $(FLAG) $(INCL) -o dbgextracter
	gdb -q dbgextracter

pg:$(SRC)
	$(CC) $? -pg $(FLAG) $(INCL) -o pgextracter

extracter:$(SRC)
	$(CC) $? $(FLAG) $(INCL) -o $@

clean:
	rm extracter
	rm dbgextracter
	rm pgextracter