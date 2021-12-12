//Made By: Derek Tomassi
//Project C
int equalsFilename(char* input, char* comparison, char* filename);
int equalsFilename2(char* input, char* comparison, char* filename, char* filename2);
void exec(char* filename);
int equals(char* input, char* comparison);
void copy(char* filename, char* filename2);
void dir();
void create();

void main() {
    char input[80];
    char filename[7];
    char filename2[7];
    char buffer[13312]; 
    int sectorsRead;  

    while(1) {                
        syscall(0, "SHELL>\r\n", 0, 0); 
        syscall(1, input, 0, 0);

        syscall(0, input, 0, 0);
        syscall(0, "\r\n", 0, 0);

        
        if (equalsFilename(input, "type", filename) == 1) {          
            syscall(3, filename, buffer, &sectorsRead);
            
            if (sectorsRead > 0) {
                syscall(0, buffer, 0, 0);
                syscall(0, "\r\n", 0, 0);        
            } else {
                syscall(0, "file not found\r\n", 0, 0);
            }
        } else if (equalsFilename(input, "exec", filename) == 1){
            exec(filename);
        } else if (equals(input, "dir") == 1) {
            dir();
        } else if (equalsFilename(input, "del", filename) == 1) {
            syscall(7, filename, 0, 0);
            syscall(0, "File Deleted!\r\n", 0, 0);
        } else if (equalsFilename2(input, "copy", filename, filename2) == 1) {
            copy(filename, filename2, buffer, &sectorsRead);
        } else if (equals(input, "create textfl") == 1) {
            create();
        } else {
            syscall(0, "Bad Command\r\n", 0, 0);
        }
    }
}

//function to check if two strings are equal and returns 1 if they are equal
int equalsFilename(char* input, char* comparison, char* filename) {
    int i = 0;
    int j;
    
    //comapring command types
    while (comparison[i] != '\0') {
        if (input[i] != comparison[i]) {
            return 0;
        }
        i++;
    }
    
    //incrementing pointer
    i++;
    
    //reading filename into filename array
    for (j = 0; j < 6; j++) {
        filename[j] = input[i];
        i++;
    }
    
    //adding \0 to end of filename array
    filename[6] = '\0';
    
    return 1;
}

int equalsFilename2(char* input, char* comparison, char* filename, char* filename2) {
    int i = 0;
    int j;
    int l;
    int m;
    int n;
    
    //comapring command types
    while (comparison[i] != '\0') {
        if (input[i] != comparison[i]) {
            return 0;
        }
        i++;
    }
    
    //incrementing pointer
    i++;
    
    //reading filename into filename array
    for (j = 0; j < 6; j++) {
        if (input[i] == '\0') {
            break;
        }        
        filename[j] = input[i];
        i++;
    }
    
    //fill in if filename is not 6 characters long
    if (j < 6) {
        for (m = j; m < 6; m++) {
            filename[m] = '\0';
        }
    }

    //increment pointer
    i++;

    //reading filename2 into filename2 array
    for (l = 0; l < 6; l++) {
        if (input[i] == '\0') {
            break;
        }        
        filename2[l] = input[i];
        i++;
    }

    //fill in if filename is not 6 characters long
    if (l < 6) {
        for (n = l; n < 6; n++) {
            filename[n] = '\0';
        }
    }
    
    //adding \0 to end of filename array
    filename[6] = '\0';
    filename2[6] = '\0';
    
    return 1;
}

void exec(char* filename) {   
    syscall(4, filename, 0, 0);

    syscall(5, 0, 0, 0);
}

int equals(char* input, char* comparison) {
    int i = 0;
    
    //comapring command types
    while (comparison[i] != '\0') {       
        if (input[i] != comparison[i]) {
            return 0;
        }
        i++;
    }

    return 1;
}

void copy(char* filename, char* filename2) {
    char fileBuffer[13312];
    int fileSectorsRead;

    syscall(3, filename, fileBuffer, &fileSectorsRead);
            
    if (fileSectorsRead <= 0) {
        syscall(0, "file not found\r\n", 0, 0);
        return;
    }

    syscall(8, fileBuffer, filename2, 3);

    syscall(0, "File Copied\r\n", 0, 0);
    
}

void dir() {
    char dir[512];
    int i;
    int j;

    syscall(2, dir, 2, 0);

    for (i = 0; i <= 512; i += 32) {
        char filename[6];
        if (dir[i] != '\0') {
            for (j = 0; j < 6; j++) {
                filename[j] = dir[i + j];
            }
            syscall(0, filename, 0, 0);
            syscall(0, "\r\n", 0, 0);
        }        
    }

    syscall(5, 0, 0, 0);
}

void create() {
    char input[80];
    char newBuffer[13312];
    int i = 0;
    int count = 0;
    int size = 0;
    int j;
    int f;
    int e;

    //do {
        //syscall(0, "Enter a line of text:\r\n", 0, 0);
        //syscall(1, input, 0, 0);

        //syscall(0, input, 0, 0);
        //syscall(0, "\r\n", 0, 0);

        //for (e = i; e < 80; e++) {
          //  newBuffer[e] = input[e];
        //}

        //i += e;

        //syscall(0, &input[0], 0, 0);
    //} while (input[1] == '\0');

    for (j = 0; j < 1000; j++) {
                
        syscall(0, "Enter a line of text: (please end sentence with a |)\r\n", 0, 0);
        syscall(1, input, 0, 0);

        syscall(0, input, 0, 0);
        syscall(0, "\r\n", 0, 0);
        
            for (e = 0; e < 80; e++) {
                newBuffer[i] = input[e];
        
                if (input[e] == '|') {
                    break;
                    size += *(&input + 1) - input;
                }
                
                if (input[e] == '\0') {
                    break;
                }
                i++;
            }
        if (input[e] == '\0') {
            break;
        }
    }
    syscall(0, &newBuffer, 0, 0);
    syscall(0, "\r\n", 0, 0);    

    syscall(8, newBuffer, "textfl", 3);
}
