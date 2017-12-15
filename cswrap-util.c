/*
 * Copyright (C) 2013-2017 Red Hat, Inc.
 *
 * This file is part of cswrap.
 *
 * cswrap is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * cswrap is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with cswrap.  If not, see <http://www.gnu.org/licenses/>.
 */

#define _GNU_SOURCE 

#include "cswrap-util.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void tag_process_name(const char *prefix, const int argc, char *argv[])
{
    /* obtain bounds of the array pointed by argv[] */
    char *beg = argv[0];
    char *end = argv[argc - 1];
    while (*end++)
        ;

    const size_t total = end - beg;
    const size_t prefix_len = strlen(prefix);
    if (total <= prefix_len)
        /* not enough space to insert the prefix */
        return;

    /* shift the contents by prefix_len to right and insert the prefix */
    memmove(beg + prefix_len, beg, total - prefix_len - 1U);
    memcpy(beg, prefix, prefix_len);
}

bool remove_self_from_path(const char *tool, char *path, const char *wrap)
{
    if (!path)
        return false;

    char *const path_orig = path;
    bool found = false;

    /* go through all paths in $PATH */
    char *term = path;
    while (term) {
        term = strchr(path, ':');
        if (term)
            /* temporarily replace the separator by zero */
            *term = '\0';

        /* concatenate dirname and basename */
        char *raw_path;
        if (!path[0])
            /* PATH="" is interpreted as PATH="." */
            raw_path = strdup(tool);
        else if (-1 == asprintf(&raw_path, "%s/%s", path, tool))
            raw_path = NULL;
        if (!raw_path)
            /* OOM */
            return false;

        /* compare the canonicalized basename with wrapper_name */
        char *exec_path = canonicalize_file_name(raw_path);
        const bool self = exec_path && STREQ(wrap, basename(exec_path));
        free(exec_path);
        free(raw_path);

        if ((path_orig != path) && !path[0])
            /* PATH=":foo" is interpreted as PATH=".:foo" */
            --path;

        /* jump to the next path in $PATH */
        char *const next = (term)
            ? (term + 1)
            : (path + strlen(path));

        if (self) {
            /* remove self from $PATH */
            memmove(path, next, 1U + strlen(next));
            found = true;
            continue;
        }

        if (term)
            /* restore the original separator */
            *term = ':';

        /* move the cursor */
        path = next;
    }

    return found;
}
