# To debug the sample/smoke test, an X11 server needs to be setup after container launch.
# Either build and run the container manually, or - if using VSCode, get the Dev Containers extension.
# Run the command: "Dev Containers: Open Folder in Container"
# For a virtual screen (headless), start it from the container with Xvfb:
#     Xvfb :99 -screen 0 1024x768x24 > /dev/null 2>&1 &
#     export DISPLAY=:99
# To connect the display to a Windows host environment, create a screen on the host with
# something like VcXsrv (disable access control), then in the container:
#     export DISPLAY=your-ip:0.0
# To debug, if VSCode/GDB - add the following environment variable to launch.json:   "environment": [{"name": "DISPLAY", "value": "your-ip:0.0"}],
FROM ubuntu:24.04

# Core Packages
RUN apt update && apt install -y \
    wget \
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
    xorg-dev \
    xvfb

# Add repo with libtinfo5
RUN wget http://security.ubuntu.com/ubuntu/pool/universe/n/ncurses/libtinfo5_6.3-2ubuntu0.1_amd64.deb
RUN apt install ./libtinfo5_6.3-2ubuntu0.1_amd64.deb

# setup python virtual environment & get setuptools
RUN python3 -m venv /opt/venv && /opt/venv/bin/pip install --upgrade setuptools

# Gfx Packages (split out for faster rebuilds, as this list is more in flux)
RUN apt install -y \
    libx11-dev \
    mesa-common-dev \
    mesa-utils \
    libxrandr-dev \
    libxinerama-dev \
    libxcursor-dev \
    libxi-dev \
    libtinfo5

# Get VulkanSDK from LunarG repo (ATM the default libvulkan-dev + vulkan-validationlayers-dev result in version mismatch)
RUN wget -qO - https://packages.lunarg.com/lunarg-signing-key-pub.asc | apt-key add - && \
    wget -qO /etc/apt/sources.list.d/lunarg-vulkan-1.4.309-noble.list https://packages.lunarg.com/vulkan/1.4.309/lunarg-vulkan-1.4.309-noble.list && \
    apt update && \
    apt install -y \
    vulkan-sdk

RUN apt-get clean && rm -rf /var/lib/apt/lists/*

ENV CC=/usr/bin/gcc-14 \
    CXX=/usr/bin/g++-14 \
    PATH="/opt/venv/bin:$PATH"
