#!/bin/bash

REPO_ROOT="${1}"
BUILD_DIR="${2}"

rm -rf "${BUILD_DIR}/energyplus_plugin"
cp -r "${REPO_ROOT}/plugin_interface/energyplus_plugin" "${BUILD_DIR}"
