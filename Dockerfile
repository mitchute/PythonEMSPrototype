FROM ubuntu:18.04
RUN apt-get update
RUN apt-get install -y --no-install-recommends locales && locale-gen en_US.UTF-8
RUN apt-get install -y --no-install-recommends python3.6-dev cmake build-essential
RUN rm -rf /var/lib/apt/lists/*
