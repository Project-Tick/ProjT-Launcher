#!/bin/sh
# Generate cmark_config.h

cat > "$1" << 'EOF'
#ifndef CMARK_CONFIG_H
#define CMARK_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#define HAVE_STDBOOL_H 1
#define HAVE___BUILTIN_EXPECT 1
#define HAVE___ATTRIBUTE__ 1

#ifdef __cplusplus
}
#endif

#endif /* CMARK_CONFIG_H */
EOF
