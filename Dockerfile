FROM debian:unstable

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
RUN cmake -DLLVM_CONFIG=/usr/bin/llvm-config .
RUN make -j8
RUN make install
CMD sleep 1 && rlwrap daleci
