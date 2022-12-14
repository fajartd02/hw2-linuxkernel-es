/*
   Name: Fajar Muhammad Hamka
   Student ID: 111012037
*/

#include <linux/fs.h> // Header for the Linux file system support
#include <linux/kernel.h> // Header for contains types, macros, functions for the kernel
#include <linux/slab.h> // Header for using kmalloc
#include <linux/uaccess.h> // Header to required for the copy to user function
#include <linux/init.h> // Header for mark up functions ex: __init __exit
#include <linux/module.h> // Core header for loading Linux Kernel Module into the kernel
#include <linux/device.h> // Header to support the kernel Driver Model
#define  DEVICE_NAME "demo" /// Define static name for device name and appear at /dev/demo using this value
#define  CLASS_NAME  "democlass" ///Define for device class, a character device driver
 
MODULE_LICENSE("GPL"); //license type, affects available functionality
MODULE_AUTHOR("Fajar M. Hamka"); // Author, will visible when you use modinfo
MODULE_DESCRIPTION("Student ID: 111012037, HW2 Embedded System"); // Description Module
 
static int    majorNumber; // Variable for store device number & make determined automatically
static int    numberOpens = 0; // Counting number open
static char   message[1000] = {0}; // Variable to memory for the string that is passed from userspace
static short  sizeOfMessage; // Size of message or memory string from userspace
static struct device* demoDevice = NULL; // Struct pointer for device driver device
static struct class*  demoClass  = NULL; // Struct pointer for device driver class

/** Because i want function in below int main(), so i should 
 *  declare function first on top, it's called prototype function 
 */
static ssize_t demo_read(struct file *, char *, size_t, loff_t *); // Function for read
static ssize_t demo_write(struct file *, const char *, size_t, loff_t *); // Function for write
static int     demo_open(struct inode *, struct file *); // Function for open
static int     demo_close(struct inode *, struct file *); // Function for close: use release in file operations kernel

// Declare file operations that we will use for file system
static struct file_operations fops = {
   .open = demo_open, // file opereations for open, refers to function demo_open
   .read = demo_read, // file opereations for read, refers to function demo_read
   .write = demo_write, // file opereations for write, refers to function demo_write
   .release = demo_close, // file opereations for close, refers to function demo_close
   .owner = THIS_MODULE // It is used by the kernel to maintain the module’s usage count.
};

// Initialization Funnction, will execute when it's running
static int __init demo_init(void) {
   printk(KERN_INFO "Demo: Initializing the Demo Linu Kernel Module\n"); // Give sign if its init works
 
   // Try to dynamically allocate a major number for the device
   majorNumber = register_chrdev(0, DEVICE_NAME, &fops);
   if (majorNumber < 0) { // will return < 0 if failed
      printk(KERN_ALERT "Demo failed to register a major number\n"); // Print if failed register
      return majorNumber; // return number int for sign, so we know number that reserved before
   }
   // Print for sign successfully register
   printk(KERN_INFO "Demo: registered correctly with major number %d\n", majorNumber);
 
   // Register the device class
   demoClass = class_create(THIS_MODULE, CLASS_NAME);
   if (IS_ERR(demoClass)) { // Check for error and clean up if there is
      unregister_chrdev(majorNumber, DEVICE_NAME); // Unregister character device if error happens
      printk(KERN_ALERT "Failed to register device class\n"); // Give a sign if failed register device class
      return PTR_ERR(demoClass); // Correct way to return an error on a pointer, so we know the error
   }
   printk(KERN_INFO "Demo: device class registered correctly\n"); // Sign if device class succesfully registered
 
   // Register the device driver
   demoDevice = device_create(demoClass, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);
   if (IS_ERR(demoDevice)) { // Clean up if there is an error
      class_destroy(demoClass); // Repeated code but the alternative is goto statements
      unregister_chrdev(majorNumber, DEVICE_NAME); // Unregsiter character device if error happens
      printk(KERN_ALERT "Failed to register the device\n"); // Sign if failed to register device
      return PTR_ERR(demoDevice); // Return pointer demo Device, so we know the error
   }
   printk(KERN_INFO "Demo: device class created correctly\n"); // Successfully! Device was initialized
   return 0; // return 0, sign for sucessfully
}


/** This function usually called cleanup function
 *  This function will be executed if it's done
 */
static void __exit demo_exit(void) {
   device_destroy(demoClass, MKDEV(majorNumber, 0)); // remove the device
   class_unregister(demoClass); // unregister the device class
   class_destroy(demoClass); // remove the device class
   unregister_chrdev(majorNumber, DEVICE_NAME); // unregister the major number
   printk(KERN_INFO "Demo: Goodbye!\n"); // Sign if done!
}

// This function will be run each time the device is opened
static int demo_open(struct inode *inodep, struct file *filep) {
   numberOpens++; // increase counting open the device
   printk(KERN_INFO "Demo: Device has been opened %d time(s)\n", numberOpens); // Sign print for open & count open
   return 0; // return 0, if successfully
}

/** This function will be run if user space read, example: data is being sent from device to user
  * After that, use copy_to_user() function to send the buffer string to user and can get or captures
  * any errors
  */
static ssize_t demo_read(struct file *filep, char *buffer, size_t len, loff_t *offset) {
   int errorCount = 0; // variable for counting error
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   errorCount = copy_to_user(buffer, message, sizeOfMessage);
 
   if (errorCount == 0) { // if not have error or equals 0, it means true
      printk(KERN_INFO "Demo: Sent %d characters to the user\n", sizeOfMessage); // sign if user success sent 
      return (sizeOfMessage = 0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "Demo: Failed to send %d characters to the user\n", errorCount);
      return -EFAULT;  // Failed, return a bad address message example: -14
   }
}

/** This function is run whenever the device is being written to from user space example:
 *  When data is sent to the device from the user, data will be copy to the message[]
 *  using the sprintf() function along with the length of the string.
 */
static ssize_t demo_write(struct file *filep, const char *buffer, size_t len, loff_t *offset) {
   int errorCount = 0; // variable for counting error
   char* tempBuffer; // variable for store kernel buffer / temp buffer

   tempBuffer = kmalloc(len, GFP_KERNEL); // Dynamic memory allocation for buffer
   errorCount = copy_from_user(tempBuffer, buffer, len); // Copy a block of data from user space, with less checking.

   sprintf(message, "%s (%zu letters)", tempBuffer, len); // appending received string with its length
   sizeOfMessage = strlen(message); // store the length of the stored message

   printk(KERN_INFO "Demo: Received %zu characters from the user\n", len); // Sign if recevied character from user
   kfree(tempBuffer); // free previously allocated memory
   return len; // Return size of length
}

// The device release function that is called whenever the device is closed/released by the userspace program
static int demo_close(struct inode *inodep, struct file *filep) {
   printk(KERN_INFO "Demo: Device successfully closed\n"); // Sign if device sucess closed
   return 0; // return 0 if success
}
 
// Below function is from linux/init.h
module_init(demo_init); // module initialization
module_exit(demo_exit); // module clean up