
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>   // for memset, strerror

int fail_code = 0;

void check_errno()
{
    if(errno) {
        perror(__func__);
        ++fail_code;
    }
}

void test_getcwd()
{
    int size = 1024;
    char buff[size];
    errno = 0;
    char * ret = getcwd(buff, size);
    printf("%s: %s\n", __func__, buff);
    if(ret != buff) {
        printf("\tERROR %s: ret != buff: %p %p\n", __func__, ret, buff);
        ++fail_code;
    }
    check_errno();
}

const uid_t uid = 100;
void test_dup()
{
    errno = 0;
    // duplicate stdio
    int new_fd = dup(1);
    if (new_fd == -1 || new_fd != 3) {
        printf("\tERROR: %s failed: ret == %d\n", __func__, new_fd);
    }
    else {
        printf("%s: %d\n", __func__, new_fd);
    }
    check_errno();
}

void test_pread() {}
void test_pwrite() {}
void test_readlinkat() {}
void test_fstat() {}
void test_exit() {}
void test_exit_group() {}
void test_set_tid_address() {}
void test_futex() {}
void test_set_robust_list() {}
void test_clock_gettime() {}
void test_tgkill() {}
void test_rt_sigprocmask() {}
void test_uname() {}
void test_getpid() {}
void test_getuid() {}
void test_geteuid() {}
void test_getgid() {}
void test_gettid() {}
void test_getegid() {}
void test_brk() {}
void test_mmap() {}
void test_mprotect() {}
void test_prlimit() {}
void test_getrandom() {}

// Pulled from https://www.man7.org/linux/man-pages/man2/statx.2.html
struct RV_statx_timestamp
{
    int64_t tv_sec;   // Seconds since the Epoch
    uint32_t tv_nsec; // Nanoseconds
};

struct RV_statx
{
    uint32_t stx_mask;
    uint32_t stx_blksize;
    uint64_t stx_attributes;
    uint32_t stx_nlink;
    uint32_t stx_uid;
    uint32_t stx_gid;
    uint16_t stx_mode;
    uint64_t stx_ino;
    uint64_t stx_size;
    uint64_t stx_blocks;
    uint64_t stx_attributes_mask;

    struct RV_statx_timestamp stx_atime;
    struct RV_statx_timestamp stx_btime;
    struct RV_statx_timestamp stx_ctime;
    struct RV_statx_timestamp stx_mtime;

    uint32_t stx_rdev_major;
    uint32_t stx_rdev_minor;
    uint32_t stx_dev_major;
    uint32_t stx_dev_minor;

    uint64_t stx_mnt_id;

    uint32_t stx_dio_mem_align;
    uint32_t stx_dio_offset_align;

    uint64_t stx_subvol;

    uint32_t stx_atomic_write_unit_min;
    uint32_t stx_atomic_write_unit_max;
    uint32_t stx_atomic_write_segments_max;
    uint32_t stx_dio_read_offset_align;
    uint32_t stx_atomic_write_unit_max_opt;
};

#define SYS_STATX 291
#define STATX_BASIC_STATS 0x000007ffU   //Ideally this should be defined in sys/stat.h,
                                        //Currently, Referred from https://codebrowser.dev/qt6/include/x86_64-linux-gnu/bits/statx-generic.h.html

int test_statx(const char *path, int expected_result) {
    errno = 0;

    struct RV_statx statxBuffer;
    memset(&statxBuffer, 0, sizeof(statxBuffer));
    int ret = syscall(SYS_STATX, AT_FDCWD, path, 0, STATX_BASIC_STATS, &statxBuffer);
    if (ret != expected_result) {
        printf("ERROR: %s statx failed for '%s' with ret=%d, errno=%d (%s)\n",
               __func__, path, ret, errno, strerror(errno));
        return -1;
    }

    if (ret == 0) {
        printf("%s: statx succeeded for '%s'\n", __func__, path);
    }

    if(ret == -1){
        printf("%s: statx failed (expected) for '%s'\n", __func__, path);
    }

    return 0;
}

void test_lstat() {}
void test_getmainvars() {}

////////////////////////////////////////////////////////////////////////////////
// File I/O operations
int test_open(const char * filename, int expect_badness, int flags, mode_t mode) {
    errno = 0;
    int fd = open(filename, flags, mode);
    if (fd == -1) {
        if (0 == expect_badness) {
            printf("\tERROR: %s failed: ret == %d\n", __func__, fd);
            check_errno();
        }
        printf("%s: successful in NOT being able to open: %s\n", __func__, filename);
    }
    else {
        printf("%s: success opening %s: fd==%d\n", __func__, filename, fd);
    }
    return fd;
}

int test_read(int fd, const int expected_bytes)
{
    errno = 0;
    char text_data[1024];
    const ssize_t num_bytes = read(fd, text_data, 1024);

    if (num_bytes != expected_bytes) {
        printf("\tERROR: %s num bytes read %ld does not match expected %d\n",
               __func__, num_bytes, expected_bytes);
        check_errno();
        ++fail_code;
        return -1;
    }
    else {
        printf("%s: successfully read %ld bytes\n", __func__, num_bytes);
    }
}

void test_lseek(int fd, const int expected_bytes)
{
    errno = 0;
    // Go to the beginning of the file
    off_t ret = lseek(fd, 0, SEEK_SET);
    if(ret == (off_t)-1) {
        printf("\tERROR: %s: could not seek to beginning of the file\n",
               __func__);
        check_errno();
        return;
    }

    if(test_read(fd, expected_bytes) == -1) {
        // test failed, test_read will print error
        printf("\tERROR: %s: after lseek, the read failed\n",
               __func__);
        return;
    }

    ret = lseek(fd, expected_bytes/2, SEEK_SET);
    if(ret == (off_t)-1) {
        printf("\tERROR: %s: could not seek to middle of the file\n",
               __func__);
        check_errno();
        return;
    }

    if(test_read(fd, expected_bytes/2) == -1) {
        // test failed, test_read will print error
        printf("\tERROR: %s: after lseek, the read failed\n",
               __func__);
        return;
    }

}
void test_close(int fd)
{
    errno = 0;
    if(close(fd) == -1) {
        printf("\tERROR: %s: close failed\n", __func__);
        check_errno();
        return;
    }
    printf("%s: successfully closed file\n", __func__);
}

void test_unlink(const char * filename)
{
    errno = 0;
    if(unlink(filename) == -1) {
        printf("\tERROR: %s: unlink failed\n", __func__);
        check_errno();
    }
    printf("%s: successfully unlinked file: %s\n", __func__, filename);
}

void test_write()
{
    errno = 0;
    char * tmp_name = tmpnam(NULL);
    if(tmp_name == NULL) {
        printf("ERROR: %s failed to create a tmpfile\n\n", __func__);
        check_errno();
        exit(1);
    }

    printf("%s: created tmp file: %s\n", __func__, tmp_name);
    int fd = test_open(tmp_name, 0, O_CREAT, S_IRWXU);
    if (fd == -1) {
        printf("\tERROR: %s: could not open the tmp file for writing: %s\n",
               __func__, tmp_name);
        check_errno();
        return;
    }

    test_close(fd);

    test_unlink(tmp_name);
}

void test_writev() {}

// Deprecated system calls or "do nothing" so no testing
void test_stime() {}
void test_ioctl() {}
void test_faccessat() {}

// To run this test, you must supply a file to read in/out
int main(int argc, char ** argv)
{
    int fd = -1;

    printf("\nRUNNING SYSCALL TEST %s\n\n", argv[0]);

    test_getcwd();
    test_dup();
    test_stime();
    test_ioctl();
    test_faccessat();

    test_open("_fake_file_name", 1, O_RDONLY, 0);

    // Test FILEIO, in this order
    if (argc == 2) {
        // The test_text.txt file should have 54 bytes of text
        const int expected_bytes = 54;

        fd = test_open(argv[1], 0, O_RDONLY, 0);
        test_read(fd, expected_bytes);
        test_lseek(fd, expected_bytes);
        test_close(fd);
    }

    test_statx(argv[1], 0);
    test_statx("/path/does/not/exist", -1);

    test_write();
    test_writev();
    test_pread();
    test_pwrite();
    test_readlinkat();
    test_exit();
    test_exit_group();
    test_set_tid_address();
    test_futex();
    test_set_robust_list();
    test_clock_gettime();
    test_tgkill();
    test_rt_sigprocmask();
    test_uname();
    test_getpid();
    test_getuid();
    test_geteuid();
    test_getgid();
    test_gettid();
    test_getegid();
    test_brk();
    test_mmap();
    test_mprotect();
    test_prlimit();
    test_getrandom();
    test_getmainvars();

    printf("\nTEST COMPLETE, return code: %d\n\n", fail_code);

    return fail_code;
}
