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
        curl && \
    pacman -Scc --noconfirm'

# Copy your project files
buildah copy $bc src /src
buildah run $bc -- git clone https://github.com/vit-vit/CTPL.git /src/external_h/CTPL

# Build your project
buildah run $bc -- bash -c 'cd /src && cmake . && make'

# Second stage: Runtime stage
rc=$(buildah from docker.io/library/archlinux:base-20230319.0.135218)
buildah config --label stage=runtime $rc

# Install runtime dependencies and clean package cache in a single RUN command
buildah run $rc -- bash -c 'pacman -Sy && \
    pacman -S --noconfirm --quiet \
        libtool \
        gcc \
        gcc-libs \
        opencv \
        hdf5 \
        netcdf-cxx \
        imagemagick \
        qt6-base \
        fmt \
        glew \
        vtk \
        jsoncpp \
        curl && \
    pacman -Scc --noconfirm'

buildah copy --from=$bc $rc /metno_gifs_cpp/src/metno_gif /usr/local/bin/metno_gif
buildah copy --from=$bc $rc /metno_gifs_cpp/infinite_run.sh /usr/local/bin/infinite_run


# Commit and remove the container
buildah commit $rc localhost/metno_gifs
buildah rm $bc $rc

