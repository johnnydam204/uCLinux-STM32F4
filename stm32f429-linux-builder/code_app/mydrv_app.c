#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>


#define MYLEDS_LED1_ON 	0
#define MYLEDS_LED1_OFF  1
#define MYLEDS_LED2_ON  2
#define MYLEDS_LED2_OFF 3


void print_usage(char *file)
{
    printf("Usage:\n");
    printf("%s <dev> <on|off>\n",file);
    printf("eg. \n");
    printf("%s led1 on\n", file);
    printf("%s led1 off\n", file);
    printf("%s led2 on\n", file);
    printf("%s led2 off\n", file);
}

int main(int argc , char** argv)
{
	int fd;

	if(argc != 3)
	{
		print_usage(argv[0]);
		return 0;
	}
	
	fd = open("/dev/mydrv" , O_RDWR|O_NONBLOCK);
	if(fd < 0)
	{
		printf("can't open /dev/mydrv\n");
		return -1;
	}

	
	
	if(!strcmp("a",argv[1]))
	{
		if(!strcmp("1", argv[2]))
		{
			ioctl(fd,0);
		}
		else if(!strcmp("2", argv[2]))
		{
			ioctl(fd,1);
		}
		else
		{
			print_usage(argv[0]);
			return 0;
		}
	}
	else if(!strcmp("b" , argv[1]))
	{
		 if (!strcmp("1", argv[2]))
		 {
		 	ioctl(fd, 4);
		 }
		 else if(!strcmp("2", argv[2]))
		 {
		 	ioctl(fd, 3);
		 }
		 else
		 {
		 	print_usage(argv[0]);
			return 0;
		 }
	}
	else
	{
	 	print_usage(argv[0]);
		return 0;
	}
	
	return 0;
}

