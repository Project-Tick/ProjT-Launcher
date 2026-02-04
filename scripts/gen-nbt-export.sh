#!/bin/sh
# Generate nbt_export.h for libnbtplusplus

cat > "$1" << 'EOF'
#ifndef NBT_EXPORT_H
#define NBT_EXPORT_H

#ifdef NBT_STATIC_DEFINE
#  define NBT_EXPORT
#  define NBT_NO_EXPORT
#else
#  ifndef NBT_EXPORT
#    ifdef nbt___EXPORTS
#      define NBT_EXPORT __attribute__((visibility("default")))
#    else
#      define NBT_EXPORT __attribute__((visibility("default")))
#    endif
#  endif
#  ifndef NBT_NO_EXPORT
#    define NBT_NO_EXPORT __attribute__((visibility("hidden")))
#  endif
#endif

#ifndef NBT_DEPRECATED
#  define NBT_DEPRECATED __attribute__ ((__deprecated__))
#endif

#ifndef NBT_DEPRECATED_EXPORT
#  define NBT_DEPRECATED_EXPORT NBT_EXPORT NBT_DEPRECATED
#endif

#ifndef NBT_DEPRECATED_NO_EXPORT
#  define NBT_DEPRECATED_NO_EXPORT NBT_NO_EXPORT NBT_DEPRECATED
#endif

#endif /* NBT_EXPORT_H */
EOF
