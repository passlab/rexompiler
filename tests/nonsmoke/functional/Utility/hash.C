// Tests various hash functions.

#include <rose.h>
#include <rosePublicConfig.h>
#include <Combinatorics.h>
#include <iostream>

using namespace Rose;

// Hash one single datum
template<class Hasher, class Data>
static void
hash(const Data &data) {
    Hasher hasher;
    hasher.insert(data);
    std::cout <<"  " <<hasher <<": " <<data <<"\n";
}

// Hash a few things singly using a particular hasher
template<class Hasher>
static void
testSingle() {
    hash<Hasher>("a");
    hash<Hasher>("abc");
    hash<Hasher>(0);
    hash<Hasher>(100);
}

// Hash a few things incrementally
template<class Hasher>
static void
testIncremental() {
    Hasher hasher;
    hasher.insert("a");
    hasher.insert("abc");
    hasher.insert(0);
    hasher.insert(100);
    std::cout <<"  " <<hasher <<": incremental\n";
}

// Test calling the hasher directly in various ways
template<class Hasher>
static void
testHasher() {
    testSingle<Hasher>();
    testIncremental<Hasher>();
}

// Test all known hash functions called directly
static void
testAllHashes() {
    std::cout <<"Fowler-Noll-Vo\n";
    testHasher<Combinatorics::HasherFnv>();

    std::cout <<"MD5\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherMd5>();
#else
    try {
        testHasher<Combinatorics::HasherMd5>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif

    std::cout <<"SHA1\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherSha1>();
#else
    try {
        testHasher<Combinatorics::HasherSha1>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif

    std::cout <<"SHA-256\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherSha256>();
#else
    try {
        testHasher<Combinatorics::HasherSha256>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif

    std::cout <<"SHA-384\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherSha384>();
#else
    try {
        testHasher<Combinatorics::HasherSha384>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif

    std::cout <<"SHA-512\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherSha512>();
#else
    try {
        testHasher<Combinatorics::HasherSha512>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif

    std::cout <<"CRC32\n";
#ifdef ROSE_HAVE_LIBGCRYPT
    testHasher<Combinatorics::HasherCrc32>();
#else
    try {
        testHasher<Combinatorics::HasherCrc32>();
        ASSERT_not_reachable("should have thrown an exception since libgcrypt is not enabled");
    } catch (const Combinatorics::Hasher::Exception&) {
    }
#endif
}

int
main() {
    testAllHashes();
}
