/***************************** 
*
*   驱动程序模板
*   版本：V1
*   使用方法(末行模式下)：
*   :%s/mydrv/"你的驱动名称"/g
*
*******************************/


#include <linux/mm.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/vmalloc.h>
#include <linux/mman.h>
#include <linux/random.h>
#include <linux/init.h>
#include <linux/raw.h>
#include <linux/tty.h>
#include <linux/capability.h>
#include <linux/ptrace.h>
#include <linux/device.h>
#include <linux/highmem.h>
#include <linux/crash_dump.h>
#include <linux/backing-dev.h>
#include <linux/bootmem.h>
#include <linux/splice.h>
#include <linux/pfn.h>
#include <linux/export.h>
#include <linux/io.h>
#include <linux/aio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>

#define MYLEDS_LED1_ON  0
#define MYLEDS_LED1_OFF 1
#define MYLEDS_LED2_ON  2
#define MYLEDS_LED2_OFF 3

volatile unsigned long *GPIOMODE;
volatile unsigned long *WAN_LED_DIR;
volatile unsigned long *WAN_LED_DATA;
volatile unsigned long *WAN_ETH0_DIR;
volatile unsigned long *WAN_ETH0_DATA;

/****************  基本定义 **********************/
//内核空间缓冲区定义
#if 0
	#define KB_MAX_SIZE 20
	#define kbuf[KB_MAX_SIZE];
#endif


//加密函数参数内容： _IOW(IOW_CHAR , IOW_NUMn , IOW_TYPE)
//加密函数用于mydrv_ioctl函数中
//使用举例：ioctl(fd , _IOW('L',0x80,long) , 0x1);
//#define NUMn mydrv , if you need!
#define IOW_CHAR 'L'
#define IOW_TYPE  long
#define IOW_NUM1  0x80


//初始化函数必要资源定义
//用于初始化函数当中
//device number;
	dev_t dev_num;
//struct dev
	struct cdev mydrv_cdev;
//auto "mknode /dev/mydrv c dev_num minor_num"
struct class *mydrv_class = NULL;
struct device *mydrv_device = NULL;


/**************** 结构体 file_operations 成员函数 *****************/
//open
static int mydrv_open(struct inode *inode, struct file *file)
{
	printk("mydrv drive open...\n");

	//*WAN_LED_DATA &= ~0x00001000;
	//*WAN_ETH0_DATA &= ~0x00000800;
	return 0;
}

//close
static int mydrv_close(struct inode *inode , struct file *file)
{
	printk("mydrv drive close...\n");


	return 0;
}

//read
static ssize_t mydrv_read(struct file *file, char __user *buffer,
			size_t len, loff_t *pos)
{
	int ret_v = 0;
	printk("mydrv drive read...\n");


	return ret_v;
}

//write
static ssize_t mydrv_write( struct file *file , const char __user *buffer,
			   size_t len , loff_t *offset )
{
	int ret_v = 0;
	printk("mydrv drive write...\n");


	return ret_v;
}

//unlocked_ioctl
static int mydrv_ioctl (struct file *filp , unsigned int cmd , unsigned long arg)
{
	int ret_v = 0;
	printk("mydrv drive ioctl...\n");

	switch(cmd)
	{
		//常规：
		//cmd值自行进行修改
		 case MYLEDS_LED1_ON:
		      *WAN_LED_DATA &= ~0x00001000;
		 break;

		 case MYLEDS_LED1_OFF:
               *WAN_LED_DATA |= 0x00001000;
		 break;

		 case MYLEDS_LED2_ON:
               *WAN_ETH0_DATA &= ~0x00000800;                       	    
		 break;
		
		 case MYLEDS_LED2_OFF:
		       *WAN_ETH0_DATA |= 0x00000800;
		 break;

		//带密码保护：
		//请在"基本定义"进行必要的定义
		case _IOW(IOW_CHAR,IOW_NUM1,IOW_TYPE):
		{
			if(arg == 0x1) //第二条件
			{
				
			}

		}
		break;

		default:
			break;
	}

	return ret_v;
}


/***************** 结构体： file_operations ************************/
//struct
static const struct file_operations mydrv_fops = {
	.owner   = THIS_MODULE,
	.open	 = mydrv_open,
	.release = mydrv_close,	
	.read	 = mydrv_read,
	.write   = mydrv_write,
	.unlocked_ioctl	= mydrv_ioctl,
};


/*************  functions: init , exit*******************/
//条件值变量，用于指示资源是否正常使用
unsigned char init_flag = 0;
unsigned char add_code_flag = 0;

//init
static __init int mydrv_init(void)
{
	int ret_v = 0;
	printk("mydrv drive init...\n");

	//函数alloc_chrdev_region主要参数说明：
	//参数2： 次设备号
	//参数3： 创建多少个设备
	if( ( ret_v = alloc_chrdev_region(&dev_num,0,1,"mydrv") ) < 0 )
	{
		goto dev_reg_error;
	}
	init_flag = 1; //标示设备创建成功；

	printk("The drive info of mydrv:\nmajor: %d\nminor: %d\n",
		MAJOR(dev_num),MINOR(dev_num));

	cdev_init(&mydrv_cdev,&mydrv_fops);
	if( (ret_v = cdev_add(&mydrv_cdev,dev_num,1)) != 0 )
	{
		goto cdev_add_error;
	}

	mydrv_class = class_create(THIS_MODULE,"mydrv");
	if( IS_ERR(mydrv_class) )
	{
		goto class_c_error;
	}

	mydrv_device = device_create(mydrv_class,NULL,dev_num,NULL,"mydrv");
	if( IS_ERR(mydrv_device) )
	{
		goto device_c_error;
	}
	printk("auto mknod success!\n");

	//------------   请在此添加您的初始化程序  --------------//
       
	GPIOMODE=(volatile unsigned long *)ioremap(0x10000064,4); //thanh ghi cho chan pinmode wanled la chan 44
    WAN_LED_DIR=(volatile unsigned long *)ioremap(0x10000604,4);//thanh ghi led 44
	WAN_LED_DATA=(volatile unsigned long *)ioremap(0x10000624,4);//thanh ghi led 44
	WAN_ETH0_DIR=(volatile unsigned long *)ioremap(0x10000604,4);//thanh ghi led 44
	WAN_ETH0_DATA=(volatile unsigned long *)ioremap(0x10000624,4);//thanh ghi led 44
	
	
	//GPIOMODE=(volatile unsigned long *)ioremap(0x10000060,4);
    //WAN_LED_DIR=(volatile unsigned long *)ioremap(0x10000674,4);
	//WAN_LED_DATA=(volatile unsigned long *)ioremap(0x10000670,4);
 	
	//*GPIOMODE |= (0x1<<14);
	//*WAN_LED_DIR |= (1<<3);
	//*WAN_LED_DIR |= (1<<4);

	
	*GPIOMODE |= 0x00000001;//cho pin wan led la gpio
	*WAN_LED_DIR |= 0x00001000;
	*WAN_LED_DATA &= ~0x00001000;
	
	*GPIOMODE |= 0x00000004;//cho pin eth led la gpio
	*WAN_ETH0_DIR |= 0x00000800;
	*WAN_ETH0_DATA &= ~0x00000800;

        //如果需要做错误处理，请：goto mydrv_error;	

	 add_code_flag = 1;
	//----------------------  END  ---------------------------// 

	goto init_success;

dev_reg_error:
	printk("alloc_chrdev_region failed\n");	
	return ret_v;

cdev_add_error:
	printk("cdev_add failed\n");
 	unregister_chrdev_region(dev_num, 1);
	init_flag = 0;
	return ret_v;

class_c_error:
	printk("class_create failed\n");
	cdev_del(&mydrv_cdev);
 	unregister_chrdev_region(dev_num, 1);
	init_flag = 0;
	return PTR_ERR(mydrv_class);

device_c_error:
	printk("device_create failed\n");
	cdev_del(&mydrv_cdev);
 	unregister_chrdev_region(dev_num, 1);
	class_destroy(mydrv_class);
	init_flag = 0;
	return PTR_ERR(mydrv_device);

//------------------ 请在此添加您的错误处理内容 ----------------//
mydrv_error:
		



	add_code_flag = 0;
	return -1;
//--------------------          END         -------------------//
    
init_success:
	printk("mydrv init success!\n");
	return 0;
}

//exit
static __exit void mydrv_exit(void)
{
	printk("mydrv drive exit...\n");	

	if(add_code_flag == 1)
 	{   
           //----------   请在这里释放您的程序占有的资源   ---------//
	    printk("free your resources...\n");	           
		//*GPIOMODE &= ~0x00000001;
		*WAN_LED_DATA |= 0x00001000;
		*WAN_ETH0_DATA |= 0x00000800;

	 	 iounmap(GPIOMODE);
		 iounmap(WAN_LED_DIR);
		 iounmap(WAN_LED_DATA);

	    printk("free finish\n");		               
	    //----------------------     END      -------------------//
	}					            

	if(init_flag == 1)
	{
		//释放初始化使用到的资源;
		cdev_del(&mydrv_cdev);
 		unregister_chrdev_region(dev_num, 1);
		device_unregister(mydrv_device);
		class_destroy(mydrv_class);
	}
}


/**************** module operations**********************/
//module loading
module_init(mydrv_init);
module_exit(mydrv_exit);

//some infomation
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("from Jafy");
MODULE_DESCRIPTION("mydrv drive");



/*********************  The End ***************************/
