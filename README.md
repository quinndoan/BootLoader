# Bootloader Simulation on KL46Z

Bootloader is a project create a bootloader program. In this project, we using chip M-KL46Z and MCUXpresso IDE.

## More Information about Bootloader Program
 - Boot Manager: A Boot Manager is a software program that is responsible for the management of the booting process of the computer. It is primarily responsible for selecting the Operating System to be loaded from multiple available options. Further, it also loads the Bootloader to perform the actual loading of the Operating System. A very popular example of Boot Manager is The BIOS boot manager.

 - Boot Loader: A Boot Loader is a software program that is responsible for “actually loading” the operating system once the Boot Manager has finished its work. Till the point Boot Loader starts loading the OS, there is nothing in the Main Memory of the machine. A very popular example of Boot Loader is Windows Boot Loader.

## Installation
 1. Install MCUXpresso IDE, you can install the IDE from NXP Official website.
 2. Install Hercule for simulating send data from PC to chip, you can visit HW Group website for download.
 3. You need a chip M-KL46Z for debugging and running code.
 4. Clone the repo and run the soure code in the MockOfficial Folder using MCUXpresso.

## Contributors
 1. Doan Thi Thu Quyen: UART, Queue, SREC, Program_Flash
 2. Nguyen Trung Dung: PORT, GPIO, Processing Mode
