# MinOS
A x86 operating system written in Rust and Assembly.<br/>
<img alt="Screenshot of the window manager, with Snake and Notepad running" src="screenshots/window%20manager.png" width="512px"/>

# **MinOS is currently being rewritten, this is the wrong branch!**

## Setting up Rust
In order for the kernel to build, you must first run:
```
rustup default nightly
rustup component add rust-src --toolchain [host toolchain, eg, nightly-x86_64-unknown-linux-gnu]
```

## Running
Just build the project and either run the iso in the build directory as a virtual machine or burn to a disk with something like:
```
sudo dd if=/path/to/MinOS.iso of=/dev/sdx && sync
```

On Qemu:
```
qemu-system-i386 -cdrom build/MinOS.iso -serial stdio -vga std
```