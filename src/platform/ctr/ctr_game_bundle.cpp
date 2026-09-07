#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <sys/stat.h>

#include "ctr_game_bundle.h"

static const char *kRomfsGameDir = "romfs:/pop2";
static const char *kInstallMarker = "sdmc:/3ds/POP2/.pop2-installed-v1";

static bool path_is_file(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISREG(st.st_mode);
}

static bool path_is_dir(const char *path) {
    struct stat st;
    return stat(path, &st) == 0 && S_ISDIR(st.st_mode);
}

static bool ensure_dir(const char *path) {
    if (path_is_dir(path)) return true;
    if (mkdir(path, 0777) == 0) return true;
    return errno == EEXIST && path_is_dir(path);
}

static bool copy_file_if_missing(const char *src, const char *dst) {
    // Never overwrite files already on SD. Some POP2 files are writable
    // (configuration/high-score/save data), so preserving them is important.
    if (path_is_file(dst)) return true;

    FILE *in = fopen(src, "rb");
    if (!in) return false;

    FILE *out = fopen(dst, "wb");
    if (!out) {
        fclose(in);
        return false;
    }

    unsigned char buffer[32768];
    bool ok = true;
    while (!feof(in)) {
        size_t got = fread(buffer, 1, sizeof(buffer), in);
        if (got && fwrite(buffer, 1, got, out) != got) {
            ok = false;
            break;
        }
        if (ferror(in)) {
            ok = false;
            break;
        }
    }

    if (fclose(out) != 0) ok = false;
    fclose(in);

    if (!ok) remove(dst);
    return ok;
}

static bool copy_tree_missing_only(const char *src_dir, const char *dst_dir) {
    DIR *dir = opendir(src_dir);
    if (!dir) return false;

    bool ok = true;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, "..")) continue;

        char src[512];
        char dst[512];
        snprintf(src, sizeof(src), "%s/%s", src_dir, entry->d_name);
        snprintf(dst, sizeof(dst), "%s/%s", dst_dir, entry->d_name);

        struct stat st;
        if (stat(src, &st) != 0) {
            ok = false;
            break;
        }

        if (S_ISDIR(st.st_mode)) {
            if (!ensure_dir(dst) || !copy_tree_missing_only(src, dst)) {
                ok = false;
                break;
            }
        } else if (S_ISREG(st.st_mode)) {
            if (!copy_file_if_missing(src, dst)) {
                ok = false;
                break;
            }
        }
    }

    closedir(dir);
    return ok;
}

bool ctr_bundled_game_ready() {
    char exe_path[512];
    snprintf(exe_path, sizeof(exe_path), "%s/%s", CTR_BUNDLED_GAME_SD_DIR, CTR_BUNDLED_GAME_EXE);
    return path_is_file(exe_path);
}

bool ctr_install_bundled_game() {
    // A completed install is immediately accepted as long as the executable
    // still exists. This keeps launch time short after the first run.
    if (path_is_file(kInstallMarker) && ctr_bundled_game_ready()) return true;

    if (!ensure_dir("sdmc:/3ds")) return false;
    if (!ensure_dir(CTR_BUNDLED_GAME_SD_DIR)) return false;

    if (!copy_tree_missing_only(kRomfsGameDir, CTR_BUNDLED_GAME_SD_DIR)) return false;
    if (!ctr_bundled_game_ready()) return false;

    FILE *marker = fopen(kInstallMarker, "wb");
    if (!marker) return false;
    fputs("Prince of Persia 2 bundled data installed.\n", marker);
    fclose(marker);
    return true;
}
