FROM artofimagination/qt-linux-compiler:v1.0.2_qt5.12.8
ENV DEBIAN_FRONTEND=noninteractive

# Basic dev setup
RUN apt-get update && apt-get install -yqq \
	build-essential \
	dumb-init \
  gdb \
	python3-pip \
  cmake \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

# Setup OpenGL, GLX, EGL
RUN apt-get update \
  && apt-get install -y -qq --no-install-recommends \
    libglvnd0 \
    libgl1 \
    libglx0 \
    libegl1 \
    libxext6 \
    libx11-6 \
    libglvnd-dev \
    libgl1-mesa-dev \
    libegl1-mesa-dev \
  && rm -rf /var/lib/apt/lists/*

# OpenCV
RUN apt-get update && apt-get install -yqq \
	libopencv-dev \
	&& apt-get clean \
	&& rm -rf /var/lib/apt/lists/*

# Env vars for the nvidia-container-runtime.
ENV NVIDIA_VISIBLE_DEVICES all
ENV NVIDIA_DRIVER_CAPABILITIES graphics,utility,compute
