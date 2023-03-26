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
    vtk

buildah run $bc -- pacman -Scc --noconfirm

buildah run $bc -- mkdir -p /metno_images
buildah run $bc -- git clone https://github.com/roar-emaus/metno_gifs_cpp.git /metno_images
buildah commit $bc localhost/create_metno_images
buildah rm $bc
