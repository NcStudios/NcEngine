FROM ubuntu:24.04

# Core Packages
RUN apt update && apt install -y \
    git \
    make \
    cmake \
    g++-14 \
    nano \
    gnupg2 \
    python3 \
    python3-pip \
    python3.12-venv \
    libvulkan-dev \
    xorg-dev \
    xvfb

# Diligent Dependencies
RUN apt install -y \
    libglx-dev \
    libglu1-mesa-dev \
    freeglut3-dev \
    mesa-common-dev \
    libx11-dev \
    mesa-utils \
    libgl-dev \
    libgl1-mesa-dev \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev

# Get libtinfo5 from universal repo
RUN echo 'Types: deb\nURIs: http://archive.ubuntu.com/ubuntu/\nSuites: lunar\nComponents: universe\nSigned-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg' > /etc/apt/sources.list.d/ubuntu.sources && \
    apt update && \
    apt install -y libtinfo5

RUN python3 -m venv /opt/venv && /opt/venv/bin/pip install --upgrade setuptools

RUN apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14 \
    PATH="/opt/venv/bin:$PATH"


# RUN apt update && apt -y install \
#     git \
#     make \
#     cmake \
#     g++-14 \
#     libvulkan-dev \
#     xorg-dev \
#     xvfb \
#     python3 \
#     python3-pip \
#     python3.12-venv \
#     libglx-dev \
#     libglu1-mesa-dev \
#     freeglut3-dev \
#     mesa-common-dev

# RUN apt -y install \
#     build-essential \
#     libx11-dev \
#     mesa-utils \
#     libgl-dev \
#     # python3-distutils \
#     libgl1-mesa-dev \
#     libxrandr-dev \
#     libxinerama-dev \
#     libxcursor-dev \
#     libxi-dev

# RUN apt install -y nano gnupg2

# # Add the Ubuntu source for 'lunar' suite to the sources list
# RUN echo 'Types: deb\nURIs: http://archive.ubuntu.com/ubuntu/\nSuites: lunar\nComponents: universe\nSigned-By: /usr/share/keyrings/ubuntu-archive-keyring.gpg' > /etc/apt/sources.list.d/ubuntu.sources

# # Update the package list
# RUN apt-get update

# # Install libtinfo5
# RUN apt-get install -y libtinfo5

# # RUN apt -y install python3 python3-pip python3-virtualenv
# RUN python3 -m venv /opt/venv
# RUN /opt/venv/bin/pip install --upgrade setuptools
# ENV PATH="/opt/venv/bin:$PATH"

# # Clean up APT when done to reduce image size
# RUN apt-get clean && rm -rf /var/lib/apt/lists/*

# ENV CC=/usr/bin/gcc-14 \
#     CXX=/usr/bin/g++-14

# To run the sample/smoke test, an X11 server needs to be setup after container launch.
# For a virtual screen (headless), start it from the container with Xvfb:
#     Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
#     export DISPLAY=:99
# To connect the display to a Windows host environment, create a screen on the host with
# something like VcXsrv (disable access control), then in the container:
#     export DISPLAY=your-ip:0.0
