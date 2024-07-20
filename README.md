# Bootloader Simulation on KL46Z

Bootloader is a project creating a simple bootloader program. This project utilizes the M-KL46Z chip and the MCUXpresso IDE.

## More Information about Bootloader Program
 - Boot Manager: A Boot Manager is a software program responsible for managing the computer's booting process. It primarily selects the operating system to load from multiple available options and loads the Bootloader to carry out the actual operating system loading. A well-known example of a Boot Manager is the BIOS boot manager.

 - Boot Loader: A Boot Loader is a software program responsible for actually loading the operating system after the Boot Manager has completed its task. Until the Boot Loader begins loading the OS, the machine's main memory remains empty. A well-known example of a Boot Loader is the Windows Boot Loader.

## Installation
 1. Install MCUXpresso IDE, you can install the IDE from NXP Official website.
 2. Install Hercule to simulate sending data from a PC to chip. You can download it from the HW Group website.
 3. You need a chip M-KL46Z for debugging and running code.
 4. Clone the repo and build project (all files in the MockOfficial Folder) using MCUXpresso.

## Contributors
 1. Doan Thi Thu Quyen: UART, Queue, SREC, Program_Flash
 2. Nguyen Trung Dung: PORT, GPIO, Flash, Processing Mode
