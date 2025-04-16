#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#define DEVICE_FILE "/dev/led_module"
int main(void)
{
    int fd = 0;
    fd = open(DEVICE_FILE, O_RDWR);
    if(fd < 0)
    {
        printf("Open file is failed\n");
        return -1;
    }
    /* Turn led */
    while(1)
    {
        write(fd,"on",3);
        usleep(500000);
        write(fd, "off",4);
        usleep(500000);
    }
}
