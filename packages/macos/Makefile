NAME      := dvs
DVS_DIR   := $(shell cd ../..; pwd)
BUILD_DIR := ${DVS_DIR}/../build-${NAME}
GIT_HASH  := $(shell cd ${DVS_DIR}; git rev-parse --short HEAD)

DVS_VERSION = $(shell cat ../../Version.txt)

all : build_pkg

clean :
	rm -rf ${BUILD_DIR}

create_build_dir :
	@echo BUILD_DIR = ${BUILD_DIR}
	@echo DVS_DIR = ${DVS_DIR}
	@echo DVS_VERSION = ${DVS_VERSION}
	@echo GIT_HASH = ${GIT_HASH}
	if [ ! -d ${BUILD_DIR} ]; then \
		mkdir -p ${BUILD_DIR}; \
		cd ${BUILD_DIR};       \
	fi

build : create_build_dir
	cd ${BUILD_DIR}; cmake -DGIT_HASH=${GIT_HASH} ${DVS_DIR};
	cd ${BUILD_DIR}; cmake --build .

build_dmg : build
	cd ${BUILD_DIR}; rm -rf app *.dmg
	cd ${BUILD_DIR}; mkdir -p app; cp src/${NAME} app
	cd ${BUILD_DIR}; ${DVS_DIR}/packages/macos/create-dmg \
		--volname "DVS ${DVS_VERSION}.${GIT_HASH} Installer" \
		"${NAME}-${DVS_VERSION}.${GIT_HASH}.dmg" \
		"${BUILD_DIR}/app"

build_pkg : build
	
	cd ${BUILD_DIR}; rm -rf pkg *.pkg
	cd ${BUILD_DIR}; mkdir -p pkg; cp src/dvs pkg/
	cd ${BUILD_DIR}; productbuild --root ${BUILD_DIR}/pkg /usr/local/bin ${BUILD_DIR}/${NAME}-${DVS_VERSION}-${GIT_HASH}.pkg
