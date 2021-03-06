#!/bin/bash -ex

ORIPATH=$(pwd)
ITEM=$1
VERSION=$2
PACKAGE=package
LIBDIR=${ORIPATH}/../build/${PACKAGE}/${ITEM}/usr/lib64/immersive-${ITEM}/
BINDIR=${ORIPATH}/../build/${PACKAGE}/${ITEM}/usr/bin/immersive-${ITEM}/
NAME=$(echo "immersive-${item}")

parameters_usage(){
    echo 'Usage: 1. <item>:           [ server, client ]'
    echo '       2. <version>:        Version of current package.'
}

package(){
    if [ ${ITEM} = "server" ] ; then
        echo 'sudo ldconfig && sudo cp /usr/bin/immersive-server/WorkerServer /root' > post
    elif [ ${ITEM} = "client" ] ; then
        echo 'sudo ldconfig' > post
    fi
    fpm \
        -f \
        -s dir \
        -t $1 \
        -n immersive-$2$3 \
        -v 1${VERSION} \
        --iteration 1.el7 \
        -C ${PACKAGE}/$2 \
        -p ${PACKAGE} \
        --after-install post
    rm -rf ./post
}

if [ "${ITEM}" = "-h" ] || [ $# != 2 ] ; then
    parameters_usage
    exit 0
fi
if [ "${ITEM}" != "server" ] && [ "${ITEM}" != "client" ] ; then
    parameters_usage
    exit 0
fi

mkdir -p ${LIBDIR}
mkdir -p ${BINDIR}

if [ ${ITEM} = "server" ] ; then
    echo `fgrep -rn "checkout" install_SVT.sh` | awk '{ print $3 }' > SVT_version
    git log | head -n 3 > git_info
    cd ../build
    cp /usr/local/lib/libglog.so.0                                    ${LIBDIR}
    cp /usr/local/lib/libthrift-0.12.0.so                             ${LIBDIR}
    cp /usr/local/lib/libthriftnb-0.12.0.so                           ${LIBDIR}
    cp /usr/local/lib64/libSvtHevcEnc.so.1                            ${LIBDIR}
    cp /usr/lib64/libopenhevc.so.1                                    ${LIBDIR}
    cp server/360SCVP/lib360SCVP.so                                   ${LIBDIR}
    cp server/VROmafPacking/libVROmafPacking.so                       ${LIBDIR}
    cp ../ffmpeg/dependency/libEncoder.so                             ${LIBDIR}
    cp ../ffmpeg/dependency/libDistributedEncoder.so                  ${LIBDIR}
    cp ../ffmpeg/dependency/WorkerServer                              ${BINDIR}
    cp server/ffmpeg/ffmpeg                                           ${BINDIR}
    mv ../external/SVT_version                                        ${BINDIR}
    mv ../external/git_info                                           ${BINDIR}
    strip ${LIBDIR}/*
    strip ${BINDIR}/WorkerServer ${BINDIR}/ffmpeg
    package rpm ${ITEM}
    package deb ${ITEM}
fi

if [ ${ITEM} = "client" ] ; then
    git log | head -n 3 > git_info
    cd ../build
    cp /usr/local/lib/libva-drm.so.2                                  ${LIBDIR}
    cp /usr/local/lib/libva-x11.so.2                                  ${LIBDIR}
    cp /usr/local/lib/libva.so.2                                      ${LIBDIR}
    cp /usr/local/lib/libglog.so.0                                    ${LIBDIR}
    cp client/360SCVP/lib360SCVP.so                                   ${LIBDIR}
    cp client/OmafDashAccess/libOmafDashAccess.so                     ${LIBDIR}
    cp client/player/render                                           ${BINDIR}
    cp ../player/config.xml                                           ${BINDIR}
    mv ../external/git_info                                           ${BINDIR}
    strip ${LIBDIR}/*
    strip ${BINDIR}/render
    package rpm ${ITEM}
    package deb ${ITEM}
fi

