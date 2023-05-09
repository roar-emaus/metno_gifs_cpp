#!/bin/bash
set -euo pipefail

# First stage: Build stage
bc=$(buildah from docker.io/library/archlinux:base-20230319.0.135218)
buildah config --label stage=build $bc

# Update package database, install build dependencies, and clean package cache in a single RUN command
buildah run $bc -- bash -c 'pacman -Sy && \
    pacman -S --noconfirm --quiet \
        base-devel \
        cmake \
        git \
        opencv \
        hdf5 \
        netcdf-cxx \
        imagemagick \
        qt6-base \
        fmt \
        glew \
        vtk \
        curl \
        gdb && \
    pacman -Scc --noconfirm'

buildah commit $bc localhost/metno_gifs_dev
buildah rm $bc

