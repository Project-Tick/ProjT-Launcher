#!/bin/sh
# Generate cmark_export.h

cat > "$1" << 'EOF'
#ifndef CMARK_EXPORT_H
#define CMARK_EXPORT_H

#ifdef CMARK_STATIC_DEFINE
#  define CMARK_EXPORT
#  define CMARK_NO_EXPORT
#else
#  if defined(_WIN32) || defined(__CYGWIN__)
#    ifdef cmark_EXPORTS
#      define CMARK_EXPORT __declspec(dllexport)
#    else
#      define CMARK_EXPORT __declspec(dllimport)
#    endif
#    define CMARK_NO_EXPORT
#  else
#    ifndef CMARK_EXPORT
#      define CMARK_EXPORT __attribute__((visibility("default")))
#    endif
#    ifndef CMARK_NO_EXPORT
#      define CMARK_NO_EXPORT __attribute__((visibility("hidden")))
#    endif
#  endif
#endif

#endif /* CMARK_EXPORT_H */
EOF
