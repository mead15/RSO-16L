FROM ubuntu:14.04
MAINTAINER Joanna Ohradka <ohradka.joanna@gmail.com>
RUN apt-get update 
RUN apt-get install -y build-essential 
RUN apt-get install -y cmake 
RUN apt-get install -y qt5-default 
RUN apt-get install -y openssl 
RUN apt-get install -y libssl-dev

ADD / /rso
WORKDIR /rso

#Compile and run application
RUN mkdir /rso/build
RUN cmake -H/rso/ -B/rso/build
RUN cd /rso/build/ && make
ENV DISPLAY :0
