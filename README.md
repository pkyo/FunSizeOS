# FunSizeOS

FunSizeOS aims to provide a hands-on experience in operating system development. This project guides you in building a
toy kernel from scratch, enhancing your understanding of the intricacies of operating systems.

## Build Environment Requirements

### Operating System

- **Supported OS:** Ubuntu 16.04.6 LTS

### Hardware

- **CPU Architecture:** X86_64

### Software

Ensure the following software packages are installed:

- **Bochs 2.7**: This is used for x86 and AMD64 emulation.
- **NASM 2.11.08**: This assembler is tailored for the Intel x86 architecture.
- **QEMU**: A versatile open-source machine emulator and virtualizer.

#### Installation Commands

- **Install Bochs 2.7**

```bash
# Extracting the bochs-2.7.tar.gz file
tar -zxvf bochs-2.7.tar.gz && cd bochs-2.7/

# Configure the build, considering the options specified.
sudo ./configure --with-x11 --with-wx --enable-all-optimizations --enable-readline --enable-debugger-gui --enable-x86-debugger --enable-a20-pin --enable-fast-function-calls --enable-debugger

# Compile the source code
make -j4 

# Install Bochs
sudo make install

# Verify Bochs installation
bochs --help
```

- **Install NASM**

```bash
sudo apt-get install -y nasm
```

- **Install QEMU**

```bash
sudo apt-get install -y qemu
```

- **Install Additional Tools**

```bash
sudo apt-get update

# The following installs a variety of necessary tools and software.
sudo apt-get install -y \
    openssh-server \
    openssh-client \
    sudo \
    software-properties-common \
    vim \
    build-essential \
    iproute2 \
    iputils-ping \
    net-tools \
    tcpdump \
    wireshark \
    wget \
    locales \
    qemu-kvm \
    libvirt-bin \
    virt-manager \
    virt-viewer \
    nasm \
    cmake \
    gdb \
    gdbserver \
    x11-apps \
    unzip \
    libx11-dev \
    libc6-dev \
    xorg-dev \
    libgtk2.0-dev \
    libreadline-dev \
    git
```

### Verification

1. **Check the Operating System**

```bash
lsb_release -d
```

Ensure the output corresponds to the specified OS version.

2. **Check the CPU Architecture**

```bash
arch
```

Ensure that the output is `x86_64`.

By following the steps above, your system should be prepared for kernel development with FunSizeOS.