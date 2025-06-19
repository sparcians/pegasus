
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

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

void test_stime() {}
void test_ioctl() {}
void test_faccessat() {}
void test_close() {}
void test_lseek() {}
void test_read() {}
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
void test_open() {}
void test_lstat() {}
void test_getmainvars() {}

int main()
{
    test_getcwd();
    test_dup();
    test_stime();
    test_ioctl();
    test_faccessat();
    test_close();
    test_lseek();
    test_read();
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
    test_open();
    test_lstat();
    test_getmainvars();
    return fail_code;
}
