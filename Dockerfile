FROM debian

# Install dependencies
RUN apt-get update -y &&\
	apt-get install -y nasm binutils make xorriso grub-common grub-pc-bin

# Link then create ISO
WORKDIR /code
CMD make