FROM ubuntu:latest

# Fix enter timezone issue
ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install build-essential valgrind -y

WORKDIR /project

# docker build -t nevyn/devtools:latest .
# docker run --rm -tiv $PWD/..:/project nevyn/devtools:latest