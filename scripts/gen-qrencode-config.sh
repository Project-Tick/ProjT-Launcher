#!/bin/sh
# Generate qrencode_config.h for libqrencode

cat > "$1" << 'EOF'
#ifndef QRENCODE_CONFIG_H
#define QRENCODE_CONFIG_H

#define HAVE_CONFIG_H 1
#define HAVE_LIBPNG 1
#define HAVE_PNG_H 1
#define HAVE_STRDUP 1
#define MAJOR_VERSION 4
#define MICRO_VERSION 1
#define MINOR_VERSION 1
#define PACKAGE "qrencode"
#define PACKAGE_BUGREPORT ""
#define PACKAGE_NAME "qrencode"
#define PACKAGE_STRING "qrencode 4.1.1"
#define PACKAGE_TARNAME "qrencode"
#define PACKAGE_URL ""
#define PACKAGE_VERSION "4.1.1"
#define STATIC_IN_RELEASE static
#define STDC_HEADERS 1
#define VERSION "4.1.1"
#define HAVE_PTHREAD 1

#endif /* QRENCODE_CONFIG_H */
EOF
