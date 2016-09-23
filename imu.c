#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <stdarg.h>
#include <poll.h>
#include <errno.h>

const char *raw_filename = "raw_data";
const char *filename = "/dev/i2c-1";
int DEBUG = 0;
int ADDRESS = 0x68;
struct pollfd pfd;

void debug(const char* format, ...)
{
    if (DEBUG)
    {
        va_list argptr;
        va_start(argptr, format);
        fprintf(stdout, "### ");
        vfprintf(stdout, format, argptr);
        va_end(argptr);
    }
}

void error_handler(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

void set_debug(const char *deb_lev)
{
    unsigned long num;
    char *p;
    errno = 0;

    num = strtoul(deb_lev, &p, 10);

    if (errno != 0 || *p != '\0')
        error_handler("set_debug | strtoul");

    DEBUG = (num > 0);
}

int open_file(const char *filename)
{
    int fd;
    
    if ((fd = open(filename, O_RDWR)) == -1)
        error_handler("open_file | open");

    debug("\"%s\" opened at %d\n", filename, fd);
    return fd;
}

int8_t read_value(int fd)
{
    debug("Reading from %d\n", fd);

    int8_t num;
    char *p = (char *)&num;
    ssize_t size = sizeof(int8_t);
    ssize_t r = 0;

    while (size > 0)
    {
        if ((r = read(fd, p, size)) == -1)
            error_handler("read_value | read");

        size -= r;
        p += r;
    }

    return num;
}

void command(uint16_t reg, int fd)
{
    debug("Writing to %d\n", fd);

    unsigned char reg_buf[2];
    ssize_t w = 0;
    ssize_t size = sizeof(unsigned char)*2;

    reg_buf[0] = (reg >> 0) & 0xFF;
    reg_buf[1] = (reg >> 8) & 0xFF;

    if ((w = write(fd, reg_buf, size)) == -1)
        error_handler("command | write");
}

void read_data_from_imu(struct pollfd *pfd)
{
    int8_t val;
    int p;

    for (;;)
    {
        command(0x3b, pfd->fd);

        switch (p = poll(pfd, 1, 100))
        {
            case -1:
                error_handler("read_data_from_imu | poll");
            case 0:
                fprintf(stderr, "Timeout expired\n");
                break;
            default:
                val = read_value(pfd->fd);
                printf("Read: %u\n", val);
                break;
        }
    }
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s debug_flag\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    set_debug(argv[1]);

    pfd.fd = open_file(filename);
    
    debug("Setting slave address\n");
    if (ioctl(pfd.fd, I2C_SLAVE, ADDRESS) == -1)
        error_handler("main | ioctl");

    read_data_from_imu(&pfd);

    return EXIT_SUCCESS;
}
