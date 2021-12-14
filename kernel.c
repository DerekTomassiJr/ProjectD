//Made By: Derek Tomassi
//Project C

void printString(char*);
char* readString(char*);
void readSector(char* buffer, int sector);
void writeSector(char* buffer, int sector);
void readFile(char* filename, char* buffer, int* sectorsRead);
void executeProgram(char* name);
void deleteFile(char* filename);
void writeFile(char* buffer, char* filename, int numberOfSectors);
void terminate();
void handleInterrupt21(int ax, int bx, int cx, int dx);

void main() { 
    makeInterrupt21();
    interrupt(0x21, 4, "shell", 0, 0);
	while(1); /*hang up*/
}

//Function takes in string and prints to the screen
void printString(char* chars) {
	int i = 0;
	while (chars[i] != 0x0) {
		interrupt(0x10, 0xe*256+chars[i], 0, 0, 0);
		i++;
	}
}

//function takes 80 character input and reads it in as input
char* readString(char* line) {
	int i = 0;
	char chars = interrupt(0x16, 0, 0, 0, 0);
	while(chars != 0xd) {
		line[i] = chars;
		i++;
		chars = interrupt(0x16, 0, 0, 0, 0);
        interrupt(0x10, 0xe*256+chars, 0, 0, 0);
		if (chars == 0x8 && i > 0) {
                        //printString(chars);
                } else if (i < 1) {
                        printString("Array Out of Bounds Error!");
                }

	}

	line[i] = 0xa;
	line[i+1] = 0x0;
	return line;
}

//function takes in 512 character array and sector number to read desired disk sector
void readSector(char* buffer, int sector) {
	int ah = 2;
	int al = 1;
	int ax = ah * 256 + al;

	int ch = 0;
	int cl = sector + 1;
	int cx = ch * 256 + cl;

	int dh = 0;
	int dl = 0x80;
	int dx = dh * 256 + dl;

	interrupt(0x13, ax, buffer, cx, dx);
}

//function takes in 512 character array and sector number to write to desired disk sector
void writeSector(char* buffer, int sector) {
    int ah = 3;
	int al = 1;
	int ax = ah * 256 + al;

	int ch = 0;
	int cl = sector + 1;
	int cx = ch * 256 + cl;

	int dh = 0;
	int dl = 0x80;
	int dx = dh * 256 + dl;

	interrupt(0x13, ax, buffer, cx, dx);
}

//function that searches the directory to find a file and reads the file into a buffer
void readFile(char* filename, char* buffer, int* sectorsRead) {
	char dir[512];	
    int entry;
	int ltr;
	int s;
    int i;
	int sector;
    int foundthefile = 1;
	readSector(dir, 2);
    
	for (entry = 0; entry <= 512; entry += 32) {
        foundthefile = 1;		
        for (ltr = 0; ltr < 6; ltr++) {		
            if (filename[ltr] != dir[ltr + entry]) {
                foundthefile = 0;
			}
		}
		if (foundthefile == 1) {
			break;
		}
	}

	if (foundthefile == 0) {
		*sectorsRead = 0;
		return;
	}
    
	for (s = 6; s < 26; s++) {
		sector = dir[entry + s];
		if (sector == 0) {
			break;
		}

		readSector(buffer, sector);
		buffer += 512;
		*sectorsRead = *sectorsRead + 1;
	}
}

void executeProgram(char* name) {
    char buffer[13312];
    int sectorsRead;
    int memOffset = 0x0;
    int i = 0;
    
    //Step #1: reading file into buffer
    interrupt(0x21, 3, name, buffer, &sectorsRead);
    
    //Step #2: putting buffer into memory
    if (sectorsRead > 0) {        
        for(i = 0; i < 13312; i++) {
        putInMemory(0x2000, memOffset, buffer[i]);
        memOffset++;
        }
    } else {
        printString("program not found");
    }    

    
    //Step #3: calling launchProgram(int segment)
    launchProgram(0x2000);
}

//function that will delete a file
void deleteFile(char* filename) {
    char directory[512];
    char map[512];
    int entry;
    int ltr;
    int foundthefile;
    int s;
    int sectorlocation;

    //reading map and directory into the arrays
    readSector(map, 1);
    readSector(directory, 2);

    //finding the file in the directory
    for (entry = 0; entry <= 512; entry += 32) {
        foundthefile = 1;		
        for (ltr = 0; ltr < 6; ltr++) {		
            if (filename[ltr] != directory[ltr + entry]) {
                foundthefile = 0;
			}
		}
		if (foundthefile == 1) {
			break;
		}
	}

    //Setting the first byte of the filename to "\0"
    directory[entry] = '\0';
    
    //changing map locations to 0
    s = 6;
    while (directory[entry + s] != 0) {
        sectorlocation = directory[entry + s];        
        map[sectorlocation] = 0;
        s++;
    }

    //writing character arrays to appropriate sectors
    writeSector(map, 1);
    writeSector(directory, 2);        
}

//function to write a file
void writeFile(char* buffer, char* filename, int numberOfSectors) {
    //creating and loading map and directory arrays
    char directory[512];
    char map[512];
    int entry;
    int ltr;
    int i;
    int j;
    int k;
    int l;
    int m;
    int sectorCounter;
    char anotherbuffer[512];

    readSector(map, 1);
    readSector(directory, 2);

    //find a free directory entry
    for (entry = 0; entry <= 512; entry += 32) {	
        if (directory[entry] == '\0') {
            break;        
        }
	}
    
    //return if no free directory entries
    if (entry == 512) {
        return;
    }
    
    //copy the name to the directory entry
    i = 0;
    while (filename[i] != "\0") {
        directory[entry + i] = filename[i];
        i++;

        if (i == 6) {
            break;
        }
    }
    
    if (i < 6) {
        for (j = i; j < 6; j++) {
            directory[entry + j] = '\0';
        }
    }

    //for each sector to be written
    for (k = 0; k < numberOfSectors; k++) {
        for (sectorCounter = 3; sectorCounter < 512; sectorCounter++) {
            if (map[sectorCounter] == 0) {
                break;
            }
        }
        
        //setting sector to 0xff
        map[sectorCounter] = "0xff";
        
        //adding that sector number to directory entry
        directory[entry + 6 + k] = sectorCounter;

        //write 512 bytes from buffer to that sector
        for (l = 0; l < 512; l++) {
            anotherbuffer[l] = buffer[(512 * k) + l];
        }

        writeSector(anotherbuffer, sectorCounter);
    }

    //fill in remaining directory entries to 0
    for (m = 6 + numberOfSectors; m < 26; m++) {
        directory[entry + m] = '\0';
    }

    //writing the map and directory back to disk
    writeSector(map, 1);
    writeSector(directory, 2);  
}

void terminate() {
    char shellname[6];

    shellname[0] = 's';
    shellname[1] = 'h';
    shellname[2] = 'e';
    shellname[3] = 'l';
    shellname[4] = 'l';
    shellname[5] = '\0';

    executeProgram(shellname);
}

//Interrupt function that will run  printString, readString, and readSector functions
void handleInterrupt21(int ax, int bx, int cx, int dx) {
	if (ax == 0) {
		printString(bx);
	} else if (ax == 1) {
		readString(bx);
	} else if (ax == 2) {
		readSector(bx, cx);
	} else if (ax == 3) {
        readFile(bx, cx, dx);
    } else if (ax == 4) {
        executeProgram(bx);     
    } else if (ax == 5) {
        terminate();
    } else if (ax == 6) {
        writeSector(bx, cx);
    } else if (ax == 7) {
        deleteFile(bx);
    } else if (ax == 8) {
        writeFile(bx, cx, dx);
    } else if (ax >= 9) {
		printString("ERROR INTERRUPT 21 INCORRECT INPUT");
	}
}
