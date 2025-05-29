CPP = g++
SRCS = LWE.cpp utils.cpp demo.cpp
EXE = lwe
PY = python

all: $(EXE)

$(EXE): $(SRCS)
	$(CPP) -o $@ $^
	@echo "Executable created: $@"

plot: plot.py
	./$(EXE) 31 1073 13 
	$(PY) $<