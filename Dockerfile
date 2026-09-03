FROM ubuntu:26.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y \
       clang-20 \
       cmake \
       ninja-build \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace
COPY ./CMakeLists.txt ./CMakeLists.txt
COPY ./CMakePresets.json ./CMakePresets.json
COPY ./include/ ./include/
COPY ./library/ ./library/
COPY ./tools/ ./tools/

RUN cmake --preset=release \
    && cmake --build ./build -j$(nproc) \
    && cmake --install ./build --prefix /workspace
