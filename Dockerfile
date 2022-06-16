FROM debian

# Install dependencies
RUN apt-get update -y &&\
	apt-get install -y nasm binutils make xorriso grub-common grub-pc-bin

# Install Rust
RUN apt-get install -y curl gcc && \
	curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly --profile minimal
ENV PATH="/root/.cargo/bin:${PATH}"
RUN rustup component add rust-src --toolchain nightly-x86_64-unknown-linux-gnu
	
# Link then create ISO
WORKDIR /code
CMD make