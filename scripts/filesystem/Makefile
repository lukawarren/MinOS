NAME := filesystem

PROJDIRS := src
INCLUDEDIRS := -I../../stdlib/include

FLAGS := -std=c++17 -O2
LINKS := -lstdc++fs

CPP := g++

$(shell mkdir -p build)

all:
	cp ../../user/pages/build/pages.bin root/pages.bin
	cp ../../user/cli/build/cli.bin root/cli.bin
	@$(CPP) $(FLAGS) src/main.cpp -o build/$(NAME).o $(INCLUDEDIRS) $(LINKS)

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build)