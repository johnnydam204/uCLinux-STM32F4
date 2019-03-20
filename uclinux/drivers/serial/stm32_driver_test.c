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

#include <linux/console.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/platform_device.h>
#include <linux/serial_core.h>
#include <linux/serial_reg.h>
#include <linux/tty.h>

#include <mach/uart.h>
#include <mach/clock.h>
#include <mach/dmaregs.h>

static int __init stm32_driver_test_init(void)
{
	int	rv = 0;

	printk(KERN_INFO "This Driver is to test\n");

	return rv;
}

/*
 * Module exit
 */
static void __exit stm32_driver_test_exit(void)
{
}

module_init(stm32_driver_test_init);
module_exit(stm32_driver_test_exit);

MODULE_AUTHOR("Viet");
MODULE_DESCRIPTION("ABCD");
MODULE_LICENSE("GPL");
