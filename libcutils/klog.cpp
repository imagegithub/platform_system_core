/*
 * Copyright (C) 2008 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cutils/klog.h>

static int klog_level = KLOG_DEFAULT_LEVEL;

int klog_get_level(void) {
    return klog_level;
}

void klog_set_level(int level) {
    klog_level = level;
}

static int __open_klog(void) {
    return TEMP_FAILURE_RETRY(open("/dev/kmsg", O_WRONLY | O_CLOEXEC));
}

#define LOG_BUF_MAX 512

void klog_writev(int level, const struct iovec* iov, int iov_count) {
    if (level > klog_level) return;

    static int klog_fd = __open_klog();
    if (klog_fd == -1) return;
    TEMP_FAILURE_RETRY(writev(klog_fd, iov, iov_count));
}

void klog_write(int level, const char* fmt, ...) {
    if (level > klog_level) return;

    char buf[LOG_BUF_MAX];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);

    buf[LOG_BUF_MAX - 1] = 0;

    struct iovec iov[1];
    iov[0].iov_base = buf;
    iov[0].iov_len = strlen(buf);
    klog_writev(level, iov, 1);
}
