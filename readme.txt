Device Name- True Random Number Generator
Type  - Character device driver

Steps - following are the command that you need to run step wise to achieve true random no. from the device.

 1. Firstly, ( >> make all ) command in terminal. -- This will compile both main.c and user.c
 2. Now run ( >> sudo insmod main.ko ) -- It will insert the module into the kernel.
 3. Then ( >> sudo chmod 777 /dev/trng_dev ) -- This will give all the permissions to the user to access the file.
 4. Then run ( >> ./a.out )
 5. Now Enter the lower limit
 6. Enter the upper limit.
  output >>> random no. within the range will be displayed.

Concept Used -
	Used kernel time in nanoseconds (including mathematical operations also to achieve more randomness) to generate a random no. and then pushed it to retreive it in the specified range.
 
