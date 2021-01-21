#include "gem_build_info.h"

#ifdef DEBUG
#define BUILD_INFO_CONFIG "(debug)"
#else
#define BUILD_INFO_CONFIG ""
#endif

static const char compiler[] = BUILD_INFO_COMPILER " " __VERSION__;
static const char revision[] = BUILD_INFO_REVISION;
static const char date[] = BUILD_INFO_DATE;
static const char machine[] = BUILD_INFO_MACHINE;
static const char release[] = BUILD_INFO_RELEASE;
static const char build_info[] = BUILD_INFO_REVISION BUILD_INFO_CONFIG
    " on " BUILD_INFO_DATE " with " BUILD_INFO_COMPILER " " __VERSION__ " by " BUILD_INFO_MACHINE;

struct GemBuildInfo gem_build_info() {
    return (struct GemBuildInfo){
        .revision = revision,
        .date = date,
        .compiler = compiler,
        .machine = machine,
        .release = release,
        .release_year = BUILD_INFO_RELEASE_YEAR,
        .release_month = BUILD_INFO_RELEASE_MONTH,
        .release_day = BUILD_INFO_RELEASE_DAY,
    };
}

const char* gem_build_info_string() { return build_info; }
