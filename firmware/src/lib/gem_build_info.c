/*
    Copyright (c) 2021 Alethea Katherine Flowers.
    Published under the standard MIT License.
    Full text available at: https://opensource.org/licenses/MIT
*/

#include "gem_build_info.h"

static const char compiler[] = BUILD_INFO_COMPILER;
static const char revision[] = BUILD_INFO_REVISION;
static const char date[] = BUILD_INFO_DATE;
static const char machine[] = BUILD_INFO_MACHINE;
static const char release[] = BUILD_INFO_RELEASE;
static const char build_info[] = BUILD_INFO_STRING;

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
