
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <linux/fcntl.h>

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

void check_errors(int ret, int expected_error, const char * file_to_test)
{
    if(expected_error)
    {
        if (ret != -1) {
            printf("ERROR: %s expected to cause an error\n", file_to_test);
            exit(1);
        }
        if(errno == 0) {
            printf("ERROR: fstatat returned -1, but errno was not set\n");
            exit(1);
        }
    }
    else {
        if (ret == -1)
        {
            printf("ERROR: %s was NOT expected to cause an error\n", file_to_test);

            if(errno == 0) {
                printf("ERROR: fstatat returned -1 incorrectly and errno was not set\n");
            }
            else {
                perror("perror reports");
            }
            exit(1);
        }
        else {
            if(errno != 0) {
                printf("ERROR: fstatat was successful but errno was set\n");
                perror("perror reports");
            }
        }
    }
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        printf("ERROR: test_fstatat requires a filename test, "
               "plus 0 or 1 to indicate expected error.\n");
        exit(1);
    }

    const char * file_to_test = argv[1];
    const int expected_error = strtoul(argv[2], NULL, 10);
    struct stat test_stat;

    errno = 0;

    printf("\nfstatat_test: sizeof stat structure: %ld\n", sizeof(struct stat));

    int ret = fstatat(AT_FDCWD, "/tmp/bogus", &test_stat, AT_SYMLINK_NOFOLLOW);
    printf("%d returned\n", ret);
    check_errors(ret, expected_error, file_to_test);



    /* //////////////////////////////////////////////////////////////////////////////// */
    /* // Test the given file using the CWD */
    /* // AT_FDCWD is -100 or 0xffffffffffffff9c */

    /* int ret = fstatat(AT_FDCWD, file_to_test, &test_stat, AT_SYMLINK_NOFOLLOW); */
    /* check_errors(ret, expected_error, file_to_test); */

    /* printf("%ld\n", test_stat.st_dev);         /\* ID of device containing file *\/ */
    /* printf("%ld\n", test_stat.st_ino);         /\* Inode number *\/ */
    /* printf("%d\n", test_stat.st_mode);        /\* File type and mode *\/ */
    /* printf("%d\n", test_stat.st_nlink);       /\* Number of hard links *\/ */
    /* printf("%d\n", test_stat.st_uid);         /\* User ID of owner *\/ */
    /* printf("%d\n", test_stat.st_gid);         /\* Group ID of owner *\/ */
    /* printf("%ld\n", test_stat.st_rdev);        /\* Device ID (if special file) *\/ */
    /* printf("%ld\n", test_stat.st_size);        /\* Total size, in bytes *\/ */
    /* printf("%d\n", test_stat.st_blksize);     /\* Block size for filesystem I/O *\/ */
    /* printf("%ld\n", test_stat.st_blocks);      /\* Number of 512B blocks allocated *\/ */
    /* /\* printf("%d\n", test_stat.st_atim);  /\\* Time of last access *\\/ *\/ */
    /* /\* printf("%d\n", test_stat.st_mtim);  /\\* Time of last modification *\\/ *\/ */
    /* /\* printf("%d\n", test_stat.st_ctim);  /\\* Time of last status change *\\/ *\/ */

    /* /\* printf("%ld\n", sizeof(test_stat.st_dev));         /\\* ID of device containing file *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_ino));         /\\* Inode number *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_mode));        /\\* File type and mode *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_nlink));       /\\* Number of hard links *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_uid));         /\\* User ID of owner *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_gid));         /\\* Group ID of owner *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_rdev));        /\\* Device ID (if special file)) *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_size));        /\\* Total size, in bytes *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_blksize));     /\\* Block size for filesystem I/O *\\/ *\/ */
    /* /\* printf("%ld\n", sizeof(test_stat.st_blocks));      /\\* Number of 512B blocks allocated *\\/ *\/ */
    /* /\* printf("%d\n", sizeof(test_stat.st_atim));  /\\* Time of last access *\\/ *\/ */
    /* /\* printf("%d\n", sizeof(test_stat.st_mtim));  /\\* Time of last modification *\\/ *\/ */
    /* /\* printf("%d\n", sizeof(test_stat.st_ctim));  /\\* Time of last status change *\\/ *\/ */

    /* //////////////////////////////////////////////////////////////////////////////// */
    /* // Test stdout/stderr */
    /* ret = fstatat(STDOUT_FILENO, "", &test_stat, AT_EMPTY_PATH); */
    /* check_errors(ret, expected_error, file_to_test); */

    /* ret = fstatat(STDERR_FILENO, "", &test_stat, AT_EMPTY_PATH); */
    /* check_errors(ret, expected_error, file_to_test); */

    /* //////////////////////////////////////////////////////////////////////////////// */
    /* // All good */
    /* printf("\nfstatat_test ran successfully\n\n"); */

    return 0;
}
