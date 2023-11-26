FROM ubuntu
ARG UID
ARG GID

RUN apt-get update && apt-get install sudo -y
RUN addgroup --gid $GID nonroot && \
    adduser --uid $UID --gid $GID --disabled-password --gecos "" nonroot && \
    echo 'nonroot ALL=(ALL) NOPASSWD: ALL' >> /etc/sudoers
RUN apt-get install wget build-essential cmake git python3 neovim -y
RUN wget -O MAVSDK.deb https://github.com/mavlink/MAVSDK/releases/download/v1.4.17/libmavsdk-dev_1.4.17_ubuntu20.04_amd64.deb
RUN apt-get install ./MAVSDK.deb -y

USER nonroot