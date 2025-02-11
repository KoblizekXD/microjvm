#include "classfile/read.h"
#include <util.h>
#include <archive.h>
#include <archive_entry.h>
#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int starts_with(const char *what, const char *with)
{
    size_t len2 = strlen(with);

    if (strncmp(what, with, len2) == 0) {
        return 1;     
    }

    return 0;
}

int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix) {
        return 0;
    }
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return 0;
    }
    
    return strcmp(str + (str_len - suffix_len), suffix) == 0;
}

size_t read_jmod(const char *java_home, const char *jmod_name, class_file ***entries)
{
    size_t s1len = strlen(java_home);
    size_t s2len = strlen(jmod_name);
    char *together = malloc(s1len + 7 + s2len + 5 + 1);
    if (together == NULL) return NULL;
    strcpy(together, java_home);
    strcat(together, "/jmods/");
    strcat(together, jmod_name);
    strcat(together, ".jmod");

    struct archive *a;
    struct archive_entry *entry;
    int r;
    size_t entry_count = 0;
    a = archive_read_new();
    archive_read_support_format_all(a);
    archive_read_support_filter_all(a);

    if ((r = archive_read_open_filename(a, together, 10240))) {  // 10KB block size
        fprintf(stderr, "Failed to open archive: %s\n", archive_error_string(a));
        free(together);
        archive_read_free(a);
        return NULL;
    }
    free(together);

    while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
        const char *csn = archive_entry_pathname(entry);
        if (!starts_with(csn, "classes/") || !ends_with(csn, ".class")) {
            archive_read_data_skip(a);
            continue;
        }
        debug_fprintf(stdout, "Loading class: %s", csn);
        size_t size = archive_entry_size(entry);
        uint8_t *data = malloc(size);
        r = archive_read_data(a, data, size);
        FILE* stream = fmemopen(data, size, "rb");

        if (stream != NULL) {
            class_file *cf = read_classfile(stream);
            *entries = realloc(entries, (entry_count + 1) * sizeof(class_file*));
            entry_count++;
            *entries[entry_count - 1] = cf;
        } else errprintf("Failed to open memory handle");

        free(data);
    }

    archive_read_free(a);
    return entry_count;
}

