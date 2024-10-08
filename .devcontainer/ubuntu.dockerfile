# To run the sample/smoke test, an X11 server needs to be setup after container launch.
# For a virtual screen (headless), start it from the container with Xvfb:
#     Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
#     export DISPLAY=:99
# To connect the display to a Windows host environment, create a screen on the host with
# something like VcXsrv (disable access control), then in the container:
#     export DISPLAY=your-ip:0.0

FROM ubuntu:24.04

# Add repo with libtinfo5
RUN echo 'Types: deb\nURIs: http://archive.ubuntu.com/ubuntu/\nSuites: lunar\nComponents: universe\nSigned-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg' >> /etc/apt/sources.list.d/ubuntu.sources

# Core Packages
RUN apt update && apt install -y \
    git \
    make \
    ninja-build \
    cmake \
    g++-14 \
    gdb \
    nano \
    gnupg2 \
    python3 \
    python3-pip \
    python3.12-venv \
    libvulkan-dev \
    xorg-dev \
    xvfb

# setup python virtual environment & get setuptools
RUN python3 -m venv /opt/venv && /opt/venv/bin/pip install --upgrade setuptools

# Gfx Packages (split out for faster rebuilds, as this list is more in flux)
RUN apt install -y \
    libx11-dev \
    libgl-dev \
    libglx-dev \
    freeglut3-dev \
    mesa-common-dev \
    mesa-utils \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libtinfo5

RUN apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14 \
    PATH="/opt/venv/bin:$PATH"
