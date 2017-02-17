#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <asm/uaccess.h>        /* for get_user and put_user */
#include <linux/time.h>		/* For Time stamping */
#include <linux/ioctl.h>

#define SUCCESS 0
#define BUF_LEN 80
#define MAJOR_NUM 243

static int Device_Open = 0;
int k,j = 0;

int Message_Ptr1,Message_Ptr2;

struct timespec ts;

#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0, int)

#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1, int)



 
static dev_t first; // variable for device number
static struct cdev c_dev; // variable for the character device structure
static struct class *cls; // variable for the device class


/***************************************************************************** 
Now, my_close(), my_open(), my_read(), my_write() functions are defined here
these functions will be called for close, open, read and write system calls respectively. 
*****************************************************************************/

static int trng_open(struct inode *pinode, struct file *pfile)
{
   
    printk(KERN_INFO "Mychar : open()\n");
    /*
     * We don't want to talk to two processes at the same time
     */
    if (Device_Open)
        return -EBUSY;

    Device_Open++;
    /*
     * Initialize the message
     */
    // Message_Ptr = Message;
    try_module_get(THIS_MODULE);
    return SUCCESS;
}	

/*static ssize_t trng_read(struct file *pfile , char __user *buffer, size_t length, loff_t *offset)
{
	// Number of bytes actually written to the buffer 
    
	printk(KERN_INFO "Mychar : read()\n");
	
	if(k == 0)
	{
		length = Message_Ptr1;
		k = k+1;	
	}
	else
	{	
		length = Message_Ptr2;
		k = 0;
	}    
	return length;
}*/

static ssize_t trng_write(struct file *pfile, const char __user *buffer, size_t length, loff_t *offset)
{
	

	printk(KERN_INFO "Mychar : write()\n");

	   /* for (i = 0; i < length && i < BUF_LEN; i++)
	        get_user(Message[i], buffer + i); */
	if(j==0)
	{ 
		Message_Ptr1 = length; 
		j = j+1;	
	}		    
	else 
	{
		Message_Ptr2 = length; 
		j = 0;
	}
    /*
     * Again, return the number of input characters used
     */
    return length;	
}	

int trng_close(struct inode *pinode, struct file *pfile)
{
	printk(KERN_INFO "Mychar : close()\n");
	    /*
	     * We're now ready for our next caller
	     */
	    Device_Open--;
	    module_put(THIS_MODULE);
	    return SUCCESS;
}
	
long device_ioctl(struct file *pfile,             /* ditto */
                  unsigned int ioctl_num,        /* number and param for ioctl */
                  unsigned long ioctl_param)
{
    int r,s,q;
    //int j;
    int temp;			//,temp2;
    int p;
    unsigned long get_time;
    int sec; 		//, hr, min, tmp1,tmp2, tmp3;
//struct timeval tv;

  
     /* Switch according to the ioctl called*/

    switch (ioctl_num) {
    case IOCTL_SET_MSG:
        /*
         * Receive a pointer to a message (in user space) and set that
         * to be the device's message.  Get the parameter given to
         * ioctl by the process.
         */
		
		temp=ioctl_param;
		
		printk(KERN_ALERT "Number is %d \n",temp);        
	        trng_write(pfile, (char *) ioctl_param,temp, 0);
		
	
         
	break; 
	
	
    case IOCTL_GET_MSG:
        /*
         * Give the current message to the calling process -
         * the parameter we got is a pointer, fill it.
         */
	
       r = Message_Ptr1;
       s = Message_Ptr2;
        /*
         * Put a zero at the end of the buffer, so it will be
         * properly terminated
         */
  	ts = current_kernel_time();     
	
	//do_gettimeofday(&tv);
	get_time = ts.tv_nsec;
	sec = get_time % 60;
 	 	

	    p = sec*86;
	   
	 
	        q = (p % (s - r));
	        p = q+r;
	        return p;
	
	    
        
		
	break;
	

    
    }

    return SUCCESS;
}
//###########################################################################################


struct file_operations trng_dev_file_operations =
{
  .owner 	= THIS_MODULE,
  .open 	= trng_open,
  .release 	= trng_close,
  /*.read 	= trng_read,*/
  .write 	= trng_write,
  .unlocked_ioctl = device_ioctl
};
 
//########## INITIALIZATION FUNCTION ##################
// STEP 1,2 & 3 are to be executed in this function ### 
static int __init mychar_init(void) 
{
	printk(KERN_INFO "Namaste: mychar driver registered");
	
	// STEP 1 : reserve <major, minor>
	if (alloc_chrdev_region(&first, 0, 1, "SANCHIT") < 0)
	{
		return -1;
	}
	
	// STEP 2 : dynamically create device node in /dev directory
    if ((cls = class_create(THIS_MODULE, "chardrv")) == NULL)
	{
		unregister_chrdev_region(first, 1);
		return -1;
	}
    if (device_create(cls, NULL, first, NULL, "trng_dev") == NULL)
	{
		class_destroy(cls);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	
	// STEP 3 : Link fops and cdev to device node
    cdev_init(&c_dev, &trng_dev_file_operations);
    if (cdev_add(&c_dev, first, 1) == -1)
	{
		device_destroy(cls, first);
		class_destroy(cls);
		unregister_chrdev_region(first, 1);
		return -1;
	}
	return 0;
}
 
static void __exit mychar_exit(void) 
{
	cdev_del(&c_dev);
	device_destroy(cls, first);
	class_destroy(cls);
	unregister_chrdev_region(first, 1);
	printk(KERN_ALERT "Bye: mychar driver unregistered\n\n");
}
 
module_init(mychar_init);
module_exit(mychar_exit);
MODULE_LICENSE("GPL");

