FROM debian

# Install dependencies
RUN apt-get update -y &&\
	apt-get install -y nasm binutils make xorriso grub-common grub-pc-bin

# Install toolchain compile dependencies
RUN apt-get install -y gcc g++ libgmp-dev libmpfr-dev libmpc-dev

# Install Rust dependencies
RUN apt-get install -y curl && \
	curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly --profile minimal
ENV PATH="/root/.cargo/bin:${PATH}"
ENV CARGO_HOME="/code/cargo"
RUN rustup component add rust-src --toolchain nightly-x86_64-unknown-linux-gnu
	
# Link then create ISO
WORKDIR /code
CMD mkdir -p cargo && make
