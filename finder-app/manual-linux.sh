#!/bin/bash
# Script outline to install and build kernel.
# Author: Siddhant Jajoo.

set -e
set -u

OUTDIR=/tmp/aeld
KERNEL_REPO=git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git
KERNEL_VERSION=v6.1.55
BUSYBOX_VERSION=1_33_1
FINDER_APP_DIR=$(realpath $(dirname $0))
ARCH=arm64
CROSS_COMPILE=aarch64-none-linux-gnu-

if [ $# -lt 1 ]
then
	echo "Using default directory ${OUTDIR} for output"
else
	OUTDIR=$1
	echo "Using passed directory ${OUTDIR} for output"
fi

mkdir -p ${OUTDIR}

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/linux-stable" ]; then
    #Clone only if the repository does not exist.
	echo "CLONING GIT LINUX STABLE VERSION ${KERNEL_VERSION} IN ${OUTDIR}"
	git clone ${KERNEL_REPO} --depth 1 --single-branch --branch ${KERNEL_VERSION}
fi
if [ ! -e ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ]; then
    cd linux-stable
    echo "Checking out version ${KERNEL_VERSION}"
    git checkout ${KERNEL_VERSION}
    
    # TODO: Add your kernel build steps here
    # deep clean the kernel source tree
    echo "Deep cleaning the kernel source tree"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} mrproper

    # configure the kernel
    echo "Configuring the kernel"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} defconfig

    # Build the kernel image
    echo "Building the kernel image"
    make -j4 ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} all

    # Build the kernel modules
    echo "Building the kernel modules"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules

    # Build the device tree blobs
    echo "Building the device tree blobs"
    make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} dtbs

echo "Kernel build completed"
fi

echo "Adding the Image in outdir"
cp ${OUTDIR}/linux-stable/arch/${ARCH}/boot/Image ${OUTDIR}

echo "Creating the staging directory for the root filesystem"
cd "$OUTDIR"
if [ -d "${OUTDIR}/rootfs" ]
then
	echo "Deleting rootfs directory at ${OUTDIR}/rootfs and starting over"
    sudo rm  -rf ${OUTDIR}/rootfs
fi

# TODO: Create necessary base directories

# Navigate in to the rootfs directory
mkdir ${OUTDIR}/rootfs
cd ${OUTDIR}/rootfs

mkdir -p bin dev etc home lib lib64 proc sbin sys tmp usr var
mkdir -p usr/bin usr/lib usr/sbin
mkdir -p var/log
echo "Created necessary base directories"

cd "$OUTDIR"
if [ ! -d "${OUTDIR}/busybox" ]
then
git clone git://busybox.net/busybox.git
    cd busybox
    git checkout ${BUSYBOX_VERSION}
    # TODO:  Configure busybox
    make distclean
    make defconfig

else
    cd busybox
fi

# TODO: Make and install busybox

make ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE}
make CONFIG_PREFIX=${OUTDIR}/rootfs ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} install
cd ${OUTDIR}/rootfs
echo "Busybox installed"

echo "Library dependencies"
#echo "Original busybox grep"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "program interpreter"
${CROSS_COMPILE}readelf -a ${OUTDIR}/rootfs/bin/busybox | grep "Shared library"



# INTERPRETER=$(${CROSS_COMPILE}readelf bin/busybox | grep "program interpreter" | awk '{print $3}')
# SHARED_LIBS=($(${CROSS_COMPILE}readelf bin/busybox | grep "Shared library" | awk '{print $5}'))

# TODO: Add library dependencies to rootfs
# for lib in $INTERPRETER "${SHARED_LIBS[@]}"; do
#     libname=$(basename "$lib")
#     cp "$lib" "${OUTDIR}/rootfs/lib64/$libname"
#     echo "Copying $libname, $lib"
# done

SYSROOT=$(${CROSS_COMPILE}gcc -print-sysroot)
sudo cp ${SYSROOT}/lib/ld-linux-aarch64.so.* ${OUTDIR}/rootfs/lib
sudo cp ${SYSROOT}/lib64/libc.so.* ${OUTDIR}/rootfs/lib64
sudo cp ${SYSROOT}/lib64/libm.so.* ${OUTDIR}/rootfs/lib64
sudo cp ${SYSROOT}/lib64/libresolv.so.* ${OUTDIR}/rootfs/lib64
echo "Library dependencies added to rootfs"

# TODO: Make device nodes
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/null c 1 3
sudo mknod -m 666 ${OUTDIR}/rootfs/dev/console c 5 1
echo "Device nodes created"


# TODO: Clean and build the writer utilicdty
cd ${FINDER_APP_DIR}
make clean
make CROSS_COMPILE=${CROSS_COMPILE}
echo "Cleaned and built the writer utility"

# TODO: Copy the finder related scripts and executables to the /home directory
# on the target rootfs
cp -r writer ${OUTDIR}/rootfs/home/
cp -r finder.sh ${OUTDIR}/rootfs/home/
cp -r finder-test.sh ${OUTDIR}/rootfs/home/
cp -r autorun-qemu.sh ${OUTDIR}/rootfs/home/
cp -r conf/ ${OUTDIR}/rootfs/home/conf/
echo "Copying finder related scripts and executables to the /home directory on the target rootfs"


# TODO: Chown the root directory
cd ${OUTDIR}/rootfs
# Make the content owned by root
sudo chown -R root:root *
echo "Root directory owned by root"

# TODO: Create initramfs.cpio.gz
# cpio -H newc -ov --owner root:root: The cpio utility reads the file list from standard input and creates an archive in the "newc" format. 
# The -o option is for creating the archive, -v is for verbose mode, 
# and --owner root:root sets the owner of all the files in the archive to root.
find . | cpio -H newc -ov --owner root:root > ${OUTDIR}/initramfs.cpio
cd ..
gzip -f initramfs.cpio
echo "Initramfs created"
