/*
 * ModSecurity, http://www.modsecurity.org/
 * Copyright (c) 2015 Trustwave Holdings, Inc. (http://www.trustwave.com/)
 *
 * You may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * If any of the files related to licensing are missing or if you have any
 * other questions related to licensing please contact Trustwave Holdings, Inc.
 * directly using the email address security@modsecurity.org.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <wordexp.h>
#include <stdint.h>
#include <inttypes.h>

#include <ctime>
#include <iostream>
#include <string>
#include <vector>

#if defined _MSC_VER
#include <direct.h>
#elif defined __GNUC__
#include <sys/types.h>
#include <sys/stat.h>
#endif

#include "src/utils/msc_system.h"

namespace modsecurity {
namespace utils {


double cpu_seconds(void) {
    /*
     * FIXME: Temporary hack to fix build on MacOS X.  Very issuficient way, but
     *      works.  Worth reimplementing using mach_absolute_time().
     */
#ifndef MACOSX
    struct timespec t;
    if (!clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &t))
        return static_cast<double>(t.tv_sec)
            + static_cast<double>(t.tv_nsec / 1000000000.0);
    else
        return static_cast<double>(clock()) /
            static_cast<double>(CLOCKS_PER_SEC);
#endif
        return 0;
}


std::string find_resource(const std::string& resource,
    const std::string& config) {
    std::ifstream *iss = NULL;

    // Trying absolute or relative to the current dir.
    iss = new std::ifstream(resource, std::ios::in);
    if (iss->is_open()) {
        iss->close();
        delete iss;
        return resource;
    }
    delete iss;

    // Trying the same path of the configuration file.
    std::string f = get_path(config) + "/" + resource;
    iss = new std::ifstream(f, std::ios::in);
    if (iss->is_open()) {
        iss->close();
        delete iss;
        return f;
    }
    delete iss;

    return std::string("");
}


std::string get_path(const std::string& file) {
    size_t found;

    found = file.find_last_of("/\\");
    if (found > 0) {
        return file.substr(0, found);
    }

    return std::string("");
}


std::vector<std::string> expandEnv(const std::string& var, int flags) {
    std::vector<std::string> vars;

    wordexp_t p;
    if (wordexp(var.c_str(), &p, flags) == false) {
        if (p.we_wordc) {
            for (char** exp = p.we_wordv; *exp; ++exp) {
                vars.push_back(exp[0]);
            }
        }
        wordfree(&p);
    }
    return vars;
}


void createDir(std::string dir, int mode) {
#if defined _MSC_VER
    _mkdir(dir.data());
#elif defined __GNUC__
    mkdir(dir.data(), mode);
#endif
}


}  // namespace utils
}  // namespace modsecurity