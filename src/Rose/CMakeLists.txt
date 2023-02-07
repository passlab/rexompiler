# This header file is only for things in the ::Rose namespace. If
# you're adding something that's not in ::Rose you're doing it wrong!

# Namespaces within ::Rose (see top comment). Every namespace will
# have a directory listed here, and every directory here is a
# namespace.
add_subdirectory(AST)

# needed to properly define ROSE_UTIL_API
add_definitions(-DROSE_UTIL_EXPORTS)

#add_dependencies(roseNamespace rosetta_generated)