#!/bin/sh
# Generate cmark_version.h

cat > "$1" << 'EOF'
#ifndef CMARK_VERSION_H
#define CMARK_VERSION_H

#define CMARK_VERSION ((0 << 24) | (31 << 16) | (1 << 8) | 0)
#define CMARK_VERSION_STRING "0.31.1"

#endif /* CMARK_VERSION_H */
EOF
