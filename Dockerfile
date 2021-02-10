FROM ubuntu:16.04
RUN apt-get update && apt-get install -y gcc-aarch64-linux-gnu build-essential python
