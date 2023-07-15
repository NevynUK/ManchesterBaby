FROM ubuntu:latest

# Fix enter timezone issue
ENV TZ=Europe/London
RUN ln -snf /usr/share/zoneinfo/$TZ /etc/localtime && echo $TZ > /etc/timezone

RUN apt-get update
RUN apt-get upgrade -y
RUN apt-get install git-core sudo build-essential cmake valgrind libcppunit-dev -y

WORKDIR /valgrind

# docker build -t nevyn/valgrind:latest .
# docker run --rm -tiv $PWD/..:/valgrind nevyn/valgrind:latest