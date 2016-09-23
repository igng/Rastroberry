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
#include <errno.h>

#define ACCEL   0x3b
#define TEMP    0x41
#define GYRO    0x43
#define MAX     0xFFFF

const char *filtname = "filtered_data";
const char *rawname = "raw_data";
const char *filename = "/dev/i2c-1";
int DEBUG = 0;
int ADDRESS = 0x68;

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

FILE *fopen_file(const char *filename)
{
    FILE *file;

    if ((file = fopen(filename, "w")) == NULL)
        error_handler("fopen_file | fopen");

    return file;
}

void write_reg(int fd, int8_t reg)
{
    char *p = (char *)&reg;

    if (write(fd, p, 1) == -1)
        error_handler("write_reg | write");
}

void read_values(int fd, int8_t *buff, int size)
{
    char *p = (char *)buff;
    int r;

    while (size > 0)
    {
        if ((r = read(fd, p, size)) == -1)
            error_handler("read_values | read");

        p += r;
        size -= r;
    }
}

void thermometer(int fd, int16_t *temp)
{
    int8_t buff[2];

    write_reg(fd, TEMP);
    read_values(fd, buff, 2);

    *temp = (buff[0] << 8) + buff[1];
}

void accelerometer(int fd, int16_t *x, int16_t *y, int16_t *z)
{
    int8_t buff[6];

    write_reg(fd, ACCEL);
    read_values(fd, buff, 6);

    *x = (buff[0] << 8) + buff[1];
    *y = (buff[2] << 8) + buff[3];
    *z = (buff[4] << 8) + buff[5];
}

void gyroscope(int fd, int16_t *x, int16_t *y, int16_t *z)
{
    int8_t buff[6];

    write_reg(fd, GYRO);
    read_values(fd, buff, 6);

    *x = (buff[0] << 8) + buff[1];
    *y = (buff[2] << 8) + buff[3];
    *z = (buff[4] << 8) + buff[5];
}

void dump_data(FILE *output, int16_t x, int16_t y, int16_t z, int flag)
{
    if (fprintf(output, "%d, %d, %d", x, y, z) < 0)
        error_handler("dump_data | fprintf");
    if (fprintf(output, "%c", (flag == 0 ? '\n' : ',')) < 0)
        error_handler("dump_data | fprintf");
}

void read_data_from_imu(int fd, FILE *raw, FILE *filt)
{
    int i;

    int16_t raw_x_acc, raw_y_acc, raw_z_acc;
    int16_t raw_x_gyro, raw_y_gyro, raw_z_gyro;
//    int16_t raw_temp;

    int16_t filt_x_acc, filt_y_acc, filt_z_acc;
    int16_t filt_x_gyro, filt_y_gyro, filt_z_gyro;
//    int16_t filt_temp;

    int sum_x_acc, sum_y_acc, sum_z_acc;
    int sum_x_gyro, sum_y_gyro, sum_z_gyro;

    sum_x_acc = sum_y_acc = sum_z_acc = 0;
    sum_x_gyro = sum_y_gyro = sum_z_gyro = 0;

    for (i = 1; i < MAX; i++)
    {
        accelerometer(fd, &raw_x_acc, &raw_y_acc, &raw_z_acc);
        gyroscope(fd, &raw_x_gyro, &raw_y_gyro, &raw_z_gyro);
        dump_data(raw, raw_x_acc, raw_y_acc, raw_z_acc, 1);
        dump_data(raw, raw_x_gyro, raw_y_gyro, raw_z_gyro, 0);

        sum_x_acc += raw_x_acc;
        sum_y_acc += raw_y_acc;
        sum_z_acc += raw_z_acc;
        sum_x_gyro += raw_x_gyro;
        sum_y_gyro += raw_y_gyro;
        sum_z_gyro += raw_z_gyro;

        filt_x_acc = sum_x_acc/i;
        filt_y_acc = sum_y_acc/i;
        filt_z_acc = sum_z_acc/i;
        filt_x_gyro = sum_x_gyro/i;
        filt_y_gyro = sum_y_gyro/i;
        filt_z_gyro = sum_z_gyro/i;

        dump_data(filt, filt_x_acc, filt_y_acc, filt_z_acc, 1);
        dump_data(filt, filt_x_gyro, filt_y_gyro, filt_z_gyro, 0);
    }
}

int main(int argc, const char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: %s debug_flag\n", argv[0]);
        return EXIT_FAILURE;
    }

    int fd;
    FILE *raw_data, *filt_data;
    
    set_debug(argv[1]);
    fd = open_file(filename);
    raw_data = fopen_file(rawname);
    filt_data = fopen_file(filtname);
    
    debug("Setting slave address\n");
    if (ioctl(fd, I2C_SLAVE, ADDRESS) == -1)
        error_handler("main | ioctl");

    read_data_from_imu(fd, raw_data, filt_data);

    return EXIT_SUCCESS;
}
