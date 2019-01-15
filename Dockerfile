FROM ubuntu:16.04

WORKDIR /home

####### Aravis
RUN apt-get update && apt-get upgrade -y && \
\
    apt-get install -y build-essential \
    git \
    autoconf \
    intltool \
    gtk-doc-tools \
    libgtk2.0-dev \
    libxml2-dev && \
\
    git clone https://github.com/AravisProject/aravis.git --branch ARAVIS_0_6_1 && \
\
    cd aravis && \
    ./autogen.sh && \
    make -j4 && \
    make install && \
    ldconfig
