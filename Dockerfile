FROM debian:stable

RUN apt-get update
RUN apt-get install -y \
    llvm \
    llvm-dev \
    cmake \
    clang \
    pkg-config \
    gdb \
    vim \
    rlwrap
RUN mkdir /build
COPY ./ /build/
WORKDIR /build
ARG Dale_VERSION_REV=0
RUN Dale_VERSION_REV=${Dale_VERSION_REV} \
    cmake \
    -DLLVM_CONFIG=/usr/bin/llvm-config \
    -DCMAKE_BUILD_TYPE=Release \
    .
RUN make -j8
RUN make install
CMD sleep 1 && rlwrap daleci
