# Download base image ubuntu 20.04

FROM ubuntu:20.04

ARG NRP_USER=bbpnrsoa
ARG NRP_GROUP=bbp-ext
ARG HOME_PARENT_FOLDER=/home

# Set environment

ENV HOME ${HOME_PARENT_FOLDER}/${NRP_USER}
ENV HOME_PARENT_FOLDER ${HOME_PARENT_FOLDER}
ENV NRP_INSTALL_DIR ${HOME}/.local

# Disable Prompt During Packages Installation

ARG DEBIAN_FRONTEND=noninteractive

# INSTALL sudo

RUN apt update -y && apt-get install -y sudo

# Set NRP_USER user

RUN mkdir -p ${HOME_PARENT_FOLDER} \
    && groupadd --gid 11860 ${NRP_GROUP} \
    && useradd --home-dir ${HOME} --create-home --uid 901325 --gid 11860 --groups ${NRP_GROUP} -ms /bin/bash ${NRP_USER} \
    && echo "${NRP_USER} ALL=(ALL) NOPASSWD: ALL" >> /etc/sudoers

# Set NRP_USER directories

RUN mkdir -p \
    ${NRP_INSTALL_DIR} \
    && chown -R ${NRP_USER}:${NRP_GROUP} $HOME

# Copy .bashrc for the NRP_USER

COPY --chown=${NRP_USER}:${NRP_GROUP} .ci/bashrc $HOME/.bashrc

# Copy requirements files

COPY --chown=${NRP_USER}:${NRP_GROUP} .ci/dependencies ${HOME}/.dependencies

# Install basic dependencies

RUN apt-get update
RUN apt-get -y install $(grep -vE "^\s*#" ${HOME}/.dependencies/apt/requirements.basic.txt  | tr "\n" " ")

# Pistache REST Server

RUN add-apt-repository ppa:pistache+team/unstable

# Gazebo

RUN sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list'
RUN wget https://packages.osrfoundation.org/gazebo.key -O - | apt-key add -

# Install CLE dependencies

RUN apt-get update
RUN apt-get -y install $(grep -vE "^\s*#" ${HOME}/.dependencies/apt/requirements.cle.txt  | tr "\n" " ")

# Fix deprecated type in OGRE (std::allocator<void>::const_pointer has been deprecated with glibc-10). Until the upstream libs are updated, use this workaround. It changes nothing, the types are the same

RUN sed -i "s/typename std::allocator<void>::const_pointer/const void*/g" /usr/include/OGRE/OgreMemorySTLAllocator.h

# Switch to NRP user

USER ${NRP_USER}
ENV USER ${NRP_USER}

# EOF
