# First stage: Build stage
bc=$(buildah from docker.io/library/archlinux:base-20230319.0.135218)
buildah config --label stage=build $bc

buildah run $bc -- pacman -Sy
buildah run $bc -- pacman -S --noconfirm --quiet \
    base-devel \
    cmake \
    git \
    opencv \
    hdf5 \
    netcdf-cxx \
    imagemagick \
    qt5-base \
    fmt \
    glew \
    vtk

# Copy your project files
buildah copy $bc src /src

# Build your project
buildah run $bc -- bash -c 'cd /src && cmake . && make'

# Second stage: Runtime stage
rc=$(buildah from docker.io/library/archlinux:base-20230319.0.135218)
buildah config --label stage=runtime $rc

# Install only runtime dependencies
buildah run $rc -- pacman -Sy
buildah run $rc -- pacman -S --noconfirm --quiet \
    opencv \
    hdf5 \
    netcdf-cxx \
    imagemagick \
    qt5-base \
    fmt \
    glew \
    vtk \
    jsoncpp

# Copy the built executable from the first stage
buildah copy --from=$bc $rc /src/create_images /app/create_images

# Clean up package cache
buildah run $rc -- pacman -Scc --noconfirm

# Commit and remove the container
buildah commit $rc localhost/create_metno_images
buildah rm $bc $rc

