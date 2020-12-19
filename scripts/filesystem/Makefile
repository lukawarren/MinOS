NAME := filesystem

PROJDIRS := src
INCLUDEDIRS := -I../../stdlib/include

FLAGS := -std=c++17 -O2 -static-libgcc -static-libstdc++

CPP := g++

$(shell mkdir -p build)

all:
	@$(CPP) $(FLAGS) src/main.cpp -o build/$(NAME).exe $(INCLUDEDIRS)

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build)