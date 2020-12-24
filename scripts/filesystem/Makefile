NAME := filesystem

PROJDIRS := src
INCLUDEDIRS := -I../../stdlib/include

FLAGS := -std=c++17 -O2
LINKS := -lstdc++fs

CPP := g++

$(shell mkdir -p build)

all:
	cp ../../user/pages/build/pages.bin root/pages.bin
	cp ../../user/wm/build/wm.bin root/wm.bin
	cp ../../user/uptime/build/uptime.bin root/uptime.bin
	@$(CPP) $(FLAGS) src/main.cpp -o build/$(NAME).o $(INCLUDEDIRS) $(LINKS)

clean:
	-@$(RM) -r $(wildcard $(OBJFILES) build)