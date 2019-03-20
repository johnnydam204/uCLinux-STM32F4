/*
 * (C) Copyright 2011
 * Emcraft Systems, <www.emcraft.com>
 * Yuri Tikhonov <yur@emcraft.com>
 *
 * Contributions
 * Jordi López <jordi.lopg@gmail.com>
 * 06-20-2012 Added termios support and console settings parsing
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * STM32 USART platform driver
 */

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
#include <linux/io.h>
#include <linux/aio.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/uaccess.h>
#include <linux/ioctl.h>
#define IOW_CHAR 'L'
#define IOW_TYPE  long
#define IOW_NUM1  0x80

#define MYLEDS_LED1_ON  0
#define MYLEDS_LED1_OFF 1
#define MYLEDS_LED2_ON  2
#define MYLEDS_LED2_OFF 3

#define STM32F2_GPIOG_BASE	(STM32_AHB1PERITH_BASE + 0x1800)
#define STM32_AHB1PERITH_BASE	(STM32_PERIPH_BASE + 0x00020000)
#define STM32_PERIPH_BASE	0x40000000
volatile unsigned long *GPIO_OUT;
volatile unsigned long *GPIO_OUTTYPE;
volatile unsigned long *GPIOG_REG;
volatile unsigned long *LED_RED_DIR;
volatile unsigned long *LED_RED_DATA;
volatile unsigned long *LED_GREEN_DIR;
volatile unsigned long *LED_GREEN_DATA;

//初始化函数必要资源定义
//用于初始化函数当中
//device number;
	dev_t dev_num;
//struct dev
	struct cdev mydrv_cdev;
//auto "mknode /dev/mydrv c dev_num minor_num"
struct class *mydrv_class = NULL;
struct device *mydrv_device = NULL;

static int mydrv_open(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "mydrv drive open...\n");
	return 0;
}

static int mydrv_close(struct inode *inode , struct file *file)
{
	printk(KERN_INFO "mydrv drive close...\n");
	return 0;
}

static ssize_t mydrv_read(struct file *file, char __user *buffer,
			size_t len, loff_t *pos)
{
	int ret_v = 0;
	printk(KERN_INFO "mydrv drive read...\n");
	return ret_v;
}


static int mydrv_ioctl (struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg) 
{
	int ret_v = 0;
	printk(KERN_INFO "mydrv drive ioctl...\n");
	
	switch(cmd)
	{
		//常规：
		//cmd值自行进行修改
		 case 0:
		      *GPIO_OUT  |= 0x00002000;
		 break;

		 case 1:
               *GPIO_OUT  &= ~0x00002000;
		 break;

		 case 4:
               *GPIO_OUT  |= 0x00004000;                      	    
		 break;
		
		 case 3:
		       *GPIO_OUT  &= ~0x00004000;
		 break;

		//带密码保护：
		//请在"基本定义"进行必要的定义
		/*case _IOW(IOW_CHAR,IOW_NUM1,IOW_TYPE):
		{
			if(arg == 0x1) //第二条件
			{
				
			}

		}
		break;*/

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
	.ioctl	= mydrv_ioctl,
};

unsigned char init_flag = 0;
unsigned char add_code_flag = 0;

static int __init mydrv_init(void)
{
	int ret_v = 0;
	printk(KERN_INFO "mydrv drive init...\n");

	//函数alloc_chrdev_region主要参数说明：
	//参数2： 次设备号
	//参数3： 创建多少个设备
	if( ( ret_v = alloc_chrdev_region(&dev_num,0,1,"mydrv") ) < 0 )
	{
		goto dev_reg_error;
	}
	init_flag = 1; //标示设备创建成功；

	printk(KERN_INFO "The drive info of mydrv:\nmajor: %d\nminor: %d\n",
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
	printk(KERN_INFO "auto mknod success!\n");
	
	GPIOG_REG=(volatile unsigned long *)ioremap(0x40021800,4); //thanh ghi cho chan pinmode wanled la chan 44
	GPIO_OUTTYPE=(volatile unsigned long *)ioremap(0x40021800+4,4); //thanh ghi cho chan pinmode wanled la chan 44
	GPIO_OUT=(volatile unsigned long *)ioremap(0x40021800+0x14,4); //thanh ghi cho chan pinmode wanled la chan 44
	
    /*LED_RED_DIR=(volatile unsigned long *)ioremap(0x10000604,4);//thanh ghi led 44
	LED_RED_DATA=(volatile unsigned long *)ioremap(0x10000624,4);//thanh ghi led 44
	LED_GREEN_DIR=(volatile unsigned long *)ioremap(0x10000604,4);//thanh ghi led 44
	LED_GREEN_DATA=(volatile unsigned long *)ioremap(0x10000624,4);//thanh ghi led 44*/
	*GPIOG_REG |= 0x14000000;//cau hinh output
	//*GPIO_OUT  |= 0x00006000;
	/*volatile unsigned long *GPIOG_REG;
	volatile unsigned long *LED_RED_DIR;
	volatile unsigned long *LED_RED_DATA;
	volatile unsigned long *LED_GREEN_DIR;
	volatile unsigned long *LED_GREEN_DATA;*/
	//------------   请在此添加您的初始化程序  --------------//

        //如果需要做错误处理，请：goto mydrv_error;	

	 add_code_flag = 1;
	//----------------------  END  ---------------------------// 

	goto init_success;

dev_reg_error:
	printk(KERN_INFO "alloc_chrdev_region failed\n");	
	return ret_v;

cdev_add_error:
	printk(KERN_INFO "cdev_add failed\n");
 	unregister_chrdev_region(dev_num, 1);
	init_flag = 0;
	return ret_v;

class_c_error:
	printk(KERN_INFO "class_create failed\n");
	cdev_del(&mydrv_cdev);
 	unregister_chrdev_region(dev_num, 1);
	init_flag = 0;
	return PTR_ERR(mydrv_class);

device_c_error:
	printk(KERN_INFO "device_create failed\n");
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
	printk(KERN_INFO "mydrv init success!\n");
	return 0;
}

/*
 * Module exit
 */
static void __exit mydrv_exit(void)
{
	printk(KERN_INFO "mydrv drive exit...\n");	

	if(add_code_flag == 1)
 	{   
           //----------   请在这里释放您的程序占有的资源   ---------//
	    printk(KERN_INFO "free your resources...\n");	           
	    printk(KERN_INFO "free finish\n");		               
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

module_init(mydrv_init);
module_exit(mydrv_exit);

MODULE_AUTHOR("Viet");
MODULE_DESCRIPTION("ABCD");
MODULE_LICENSE("GPL");
