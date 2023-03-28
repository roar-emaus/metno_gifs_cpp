bc=$(buildah from docker.io/library/archlinux:base-20230319.0.135218)

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
    vtk \
    vim

buildah run $bc -- pacman -Scc --noconfirm

buildah commit $bc localhost/create_metno_images_dev
buildah rm $bc
