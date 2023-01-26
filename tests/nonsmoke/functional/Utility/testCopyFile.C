#include <rose.h>

#define SOURCE_FILE_NAME "copyFile-source"
#define TARGET_FILE_NAME "copyFile-target"
#define FILE_CONTENT "Contents of the file"             // No line feed

using namespace Rose;

static void
createSourceFile() {
    std::ofstream out(SOURCE_FILE_NAME);
    out <<FILE_CONTENT;
    ASSERT_require(out.good());
}

static void
copySourceToTarget() {
    Rose::FileSystem::copyFile(SOURCE_FILE_NAME, TARGET_FILE_NAME);
}

static void
checkTargetFile() {
    std::ifstream in(TARGET_FILE_NAME);
    char buf[256];
    in.getline(buf, sizeof buf);
    ASSERT_require(in.eof());
    ASSERT_require2(strcmp(buf, FILE_CONTENT)==0, "got \"%s\"\n", StringUtility::cEscape(buf).c_str());
}

int main() {
    createSourceFile();
    copySourceToTarget();
    checkTargetFile();
}
