# SPDX-License-Identifier: GPL-2.0

include mk/config.mk

HOST_OS ?= $(if $(filter Windows_NT,$(OS)),windows,$(shell uname -s | tr '[:upper:]' '[:lower:]'))
HOST_ARCH ?= $(shell uname -m)

TARGET_OS ?= $(HOST_OS)
TARGET_ARCH ?= $(HOST_ARCH)
