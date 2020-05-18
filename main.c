#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/**
 * This program extracts some relevant informations from a mbr file.
 * The file name is put on the first argument when the program is executed.
 *
 * @author Mohamed Ahmed
 * @version 1.0
 */

void read_partition_entry(int i, unsigned char buff[1024]) {
  unsigned int data;
  
  printf("Byte status: ");
  switch((int)buff[i]) {
  case 0x00:
    puts("Bootable partition");
    break;
  case 0x80:
    puts("Non-bootable partition");
    break;
  default:
    puts("Invalid bootable partition");
    break;
  }
  
  printf("Partition type: ");
  //These are the main common partitions
  switch((int)buff[i+4]) {
  case 0x0:
    puts("Empty parition entry");
    break;
  case 0x1:
    puts("FAT12 partition");
    break;
  case 0x14:
  case 0x4:
    puts("FAT16 partition");
    break;
  case 0x17:
  case 0x7:
    puts("NTFS partition");
    break;
  case 0xc:
    puts("FAT32 partition");
    break;
  case 0x12:
    puts("Recovery partition");
    break;
  case 0xFF:
    puts("XENIX bad block table");
    break;
  default:
    puts("Unrecognized partition type");	
  }
  {
    i += 1;
    data = ((int)buff[i+2]<<16) | ((int)buff[i+1]<<8) | (int)buff[i];
    printf("CHS address of first absolute sector: 0x%2.2X \n", data);
  }
  {
    i += 5;
    data = ((int)buff[i+2]<<16) | ((int)buff[i+1]<<8) | (int)buff[i];
    printf("CHS address of last absolute sector: 0x%2.2X \n", data);
  }
  {
    i += 8;
    data = ((int)buff[i+3]<<24) | ((int)buff[i+2]<<16) | ((int)buff[i+1]<<8) | (int)buff[i];
    printf("LBA of first absolute sector: 0x%2.2X \n", data);
  }
  {
    i += 12;
    data = (buff[i+3]<<24) | (buff[i+2]<<16) | (buff[i+1]<<8) | buff[i];
    printf("Number of sectors in partition: %d \n", data);
  }
}
int main(int argc, char* argv[]) {
  
  unsigned char buff[1024];
  FILE *file;
  int i, j, z, partition_found;
  partition_found = 0;
  
  if (argc != 2) {
    printf("Please insert one argument as the name file\n");
    return EXIT_FAILURE;
  }
  file = fopen(argv[1],"rb");
  if (file == NULL) {
    perror("fopen");
    return EXIT_FAILURE;
  }
  
  printf("Name file: %s\n", argv[1]);
  
  const size_t file_size = fread(buff, sizeof(unsigned char), 1024, file);
  
  printf("File size = %ld bytes \n", file_size);
  printf("\nSearch for strings in the file:\n");
  char exe[1024] = "strings ";
  strcat(exe, argv[1]);
  system(exe);
  
  for (i = 0; i < (file_size / sizeof(unsigned char)); i++) {
    //Extraction of data
    unsigned int data;
    switch(i) {
    case 0x1BE:
    case 0x1CE:
    case 0x1DE:
    case 0x1EE:
      z = 0;
      //Determines if the partition entry doesn't exist
      for (j = 0; j < 16; j++) {
	if (buff[i+j] == 0) z++;
      }
      if (z != 16) {
	printf("\nPartition number %d:\n", ++partition_found);
	read_partition_entry(i, buff);
      }
      break;
    case 0x1B8:      
      data = ((int)buff[i+3]<<24) | ((int)buff[i+2]<<16) | ((int)buff[i+1]<<8) | (int)buff[i];
      printf("\nDisk signature: %d \n", data);
      break;
    case 0x1BC:
      if (((int)buff[i+1]<<8 | (int)buff[i]) == 0x5A5A)
	puts("This disk is copy-protected");
      else
	puts("This disk is not copy-protected");
      break;
    case 0x1FE:
      data = ((int)buff[i+1]<<8) | (int)buff[i];
      if (data == 0xAA55)
	printf("\nMBR signature: 0x%2.2X \n", data);
      else
	puts("\nBad MBR signature at addresses 0x1FE 0x1FF");
      break;
    }
      
    /* read data
       if ((i % 8) == 0)
       printf("\t");
       if ((i % 16) == 0)
       printf("\n");
       printf("0x%2.2X ", (int)buff[i]);
    */
    
  }
  printf("Number of partitions found: %d \n", partition_found);
  fclose(file);
  
  return 0;
}
