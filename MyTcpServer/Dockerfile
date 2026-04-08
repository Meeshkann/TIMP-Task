FROM ubuntu:24.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    qt6-base-dev \
    qt6-base-dev-tools \
    libsqlite3-dev \
    libqt6core6 \
    libqt6network6 \
    nano \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY . .

RUN qmake6 && make

EXPOSE 54678

CMD ["./MyTcpServer"]