NAME := filesystem

PROJDIRS := src
INCLUDEDIRS := -I../../stdlib/include

FLAGS := -std=c++17 -O2
LINKS := -lstdc++fs

CPP := g++

$(shell mkdir -p build)

all:
	cp ../../user/pages/build/pages root/pages
	cp ../../user/cli/build/cli root/cli
	cp ../../user/ls/build/ls root/ls
	@$(CPP) $(FLAGS) src/main.cpp -o build/$(NAME).o $(INCLUDEDIRS) $(LINKS)

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build)