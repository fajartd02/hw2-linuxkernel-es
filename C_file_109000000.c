/*
   Name: Fajar Muhammad Hamka
   Student ID: 111012037
*/

#include<stdio.h> // Standard function C, like for printf
#include<errno.h> // Header for return error number
#include<fcntl.h> // Header for use open
#include<string.h> // Header for using string function
#include<unistd.h> // Header defines miscellaneous symbolic constants and types, and declares miscellaneous functions
 
#define BUFFER_LENGTH 1000 // Define buffer length
static char receiveMessage[BUFFER_LENGTH]; // variable receiveMessage buffer from the LKM
 
int main() {
   int fileReturn; // variable for signing return after operations
   int fileDirectory; // variable for temp file directory
   char messageSend[BUFFER_LENGTH]; // variable for sent a message
   printf("Starting device code...\n"); // Sign if program init

   fileDirectory = open("/dev/demo", O_RDWR); // Open the device with read/write access
   if (fileDirectory < 0) { // return < 0 if failed open
      perror("Failed to open the device..."); // Sign in for failed open
      return errno; // return error number
   }

   printf("Send message to the kernel module:\n"); // Print explanation to make clear gui
   scanf("%[^\n]%*c", messageSend); // Read in a string (with spaces)
   printf("Writing message to the device: [\"%s\"].\n", messageSend); // Printing message

   // Send the string to the linux kernel module
   fileReturn = write(fileDirectory, messageSend, strlen(messageSend));
   if (fileReturn < 0) { // return < 0 if failed write
      perror("Failed to write the message to the device."); // sign for failed write
      return errno; // return error number
   }
 
   printf("Press ENTER to read again from the device...\n"); // print for make clear ui
   getchar(); // function for get one char (we hopes user click enter)
 
   printf("Reading from the device...\n"); // Sign if we reading now
   // Read the response from Linux Kernel Module
   fileReturn = read(fileDirectory, receiveMessage, BUFFER_LENGTH);
   if (fileReturn < 0) { // return < 0 if failed read
      perror("Failed to read the message from the device."); // sign if failed read message
      return errno; // return error number
   }
   printf("Received message: [\"%s\"].\n", receiveMessage);
   printf("Thank you - Fajar Muhammad Hamka - 111012037\n");
   return 0;
}