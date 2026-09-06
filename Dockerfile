# build targets
# -----------------------------------------------------------------------------
# https://serverfault.com/a/797318 for ARG DEBIAN_FRONTEND=noninteractive

FROM ubuntu:latest AS alias_builder
ARG DEBIAN_FRONTEND=noninteractive

# Build images with things installed
# -----------------------------------------------------------------------------
FROM alias_builder as builder
RUN apt-get update
RUN apt install -y libsdl3-dev libassimp-dev curl
RUN apt install -y doxygen
RUN apt install -y gcc clang-22 cmake ninja-build dotnet-sdk-10.0
RUN apt install -y clang-tidy-22 git time



FROM builder as dist
WORKDIR /euph
ADD . /euph/
RUN chmod +x docker-build.sh
RUN ./docker-build.sh

CMD ["/bin/bash"]
