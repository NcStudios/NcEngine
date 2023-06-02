FROM ubuntu:22.04

RUN apt update && apt -y install \
    git \
    make \
    cmake \
    g++-12 \
    libvulkan-dev \
    xorg-dev

ENV CC=/usr/bin/gcc-12 \
    CXX=/usr/bin/g++-12
