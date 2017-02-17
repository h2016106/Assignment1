/*
 *  ioctl.c - the process to use ioctl's to control the kernel module
 *
 *  Until now we could have used cat for input and output.  But now
 *  we need to do ioctl's, which require writing our own process.
 */

/*
 * device specifics, such as ioctl numbers and the
 * major device file.
 */
#define DEVICE_FILE_NAME "/dev/trng_dev" 

#define MAJOR_NUM 243

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>              /* open */
#include <unistd.h>             /* exit */

#include <sys/ioctl.h>          /* ioctl */



#define IOCTL_SET_MSG _IOR(MAJOR_NUM, 0,int)

#define IOCTL_GET_MSG _IOR(MAJOR_NUM, 1,int)

/*
 * Functions for the ioctl calls
 */


int ioctl_set_msg(int file_desc,int min)
{
    int ret_val; 									

    ret_val = ioctl(file_desc, IOCTL_SET_MSG, min);
    											
    if (ret_val < 0) {
        printf("ioctl_set_msg failed:%d \n", ret_val);					
        exit(-1);
   		     }
    return 0;
}


int ioctl_get_msg(int file_desc)
{
    int ret_val;

    /*
     * Warning - this is dangerous because we don't tell
     * the kernel how far it's allowed to write, so it
     * might overflow the buffer. In a real production
     * program, we would have used two ioctls - one to tell
     * the kernel the buffer length and another to give
     * it the buffer to fill
     */
    ret_val = ioctl(file_desc, IOCTL_GET_MSG,0);

    if (ret_val < 0) {
        printf("ioctl_get_msg failed:%d\n", ret_val);
        exit(-1);
    }

   printf("Random Number:%d\n", ret_val);
    
    return 0;
}


/*
 * Main - Call the ioctl functions
 */
int main()
{

    int file_desc;

    int min,max;

    file_desc = open(DEVICE_FILE_NAME, 0);
    
    if (file_desc < 0) {
        printf("Can't open device file: %s\n", DEVICE_FILE_NAME);
        exit(-1);
    }

    printf("Enter the lower number  :");
    scanf("%d",&min);
    
    printf("Enter the upper number  :");
    scanf("%d",&max);

    ioctl_set_msg(file_desc,min);
    ioctl_set_msg(file_desc,max);  
    ioctl_get_msg(file_desc);

    close(file_desc);
    return 0;

}
