FROM ubuntu:24.04

RUN apt update && apt -y install \
    git \
    make \
    cmake \
    g++-14 \
    libvulkan-dev \
    xorg-dev

ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14
