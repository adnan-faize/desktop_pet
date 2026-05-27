FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y --no-install-recommends \
    # core build tools
    build-essential \
    git \
    # meson and ninja
    meson \
    ninja-build \
    # native linux C development
    gcc \
    libc6-dev \
    # windows cross-compilation toolchain
    mingw-w64 \
    gcc-mingw-w64-i686 \
    # testing windows binaries on linux
    wine64 \
    # documentation generation
    doxygen \
    graphviz \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

CMD ["/bin/bash"]
