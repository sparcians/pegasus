
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <limits.h>
#include <stdlib.h>

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
void test_statx() {}
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
    test_statx();
    test_getmainvars();

    printf("\nTEST COMPLETE, return code: %d\n\n", fail_code);

    return fail_code;
}
