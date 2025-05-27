CPP = g++
SRCS = LWE.cpp utils.cpp demo.cpp
EXE = lwe

all: $(EXE)

$(EXE): $(SRCS)
	$(CPP) -o $@ $^
	@echo "Executable created: $@"