
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
    printf("getcwd: %s\n", buff);
    if(ret != buff) {
        printf("ERROR %s: ret != buff: %p %p\n", __func__, ret, buff);
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
        printf("ERROR: %s failed: ret == %d\n", __func__, new_fd);
    }
    else {
        printf("%s: %d\n", __func__, new_fd);
    }
    check_errno();
}

void test_write() {}
void test_writev() {}
void test_pread() {}
void test_pwrite() {}
void test_readlinkat() {}
void test_fstatat() {}
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
int test_open(const char * filename, int expect_badness) {
    errno = 0;
    int fd = open(filename, O_RDONLY);
    if (fd == -1) {
        if (0 == expect_badness) {
            printf("ERROR: %s failed: ret == %d\n", __func__, fd);
            check_errno();
        }
        printf("%s successful in NOT being able to open: %s\n", __func__, filename);
    }
    else {
        printf("%s success: fd==%d\n", __func__, fd);
    }
    return fd;
}
void test_read(int fd) {}
void test_lseek(int fd) {}
void test_close(int fd) {}


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

    test_open("_fake_file_name", 1);

    // Test FILEIO, in this order
    if (argc == 2) {
        fd = test_open(argv[1], 0);
        test_read(fd);
        test_lseek(fd);
        test_close(fd);
    }

    test_write();
    test_writev();
    test_pread();
    test_pwrite();
    test_readlinkat();
    test_fstatat();
    test_fstat();
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
    test_lstat();
    test_getmainvars();

    printf("\nTEST COMPLETE, return code: %d\n\n", fail_code);

    return fail_code;
}
