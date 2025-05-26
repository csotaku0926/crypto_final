CPP = g++
SRCS = lwe.cpp utils.cpp
EXE = lwe

all: $(EXE)

$(EXE): $(SRCS)
	$(CPP) -o $@ $^
	@echo "Executable created: $@"