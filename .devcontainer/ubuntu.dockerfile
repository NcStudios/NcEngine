FROM ubuntu:24.04

RUN apt update && apt -y install \
    git \
    make \
    cmake \
    g++-14 \
    libvulkan-dev \
    xorg-dev \
    xvfb \
    python3 \
    python3-pip \
    libglx-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    mesa-common-dev

ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14

# To run the sample/smoke test, an X11 server needs to be setup after container launch.
# For a virtual screen (headless), start it from the container with Xvfb:
#     Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
#     export DISPLAY=:99
# To connect the display to a Windows host environment, create a screen on the host with
# something like VcXsrv (disable access control), then in the container:
#     export DISPLAY=your-ip:0.0
