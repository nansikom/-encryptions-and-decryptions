#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>  // ssize_t
#include <sys/socket.h> // send(),recv()
#include <netdb.h>      // gethostbyname()
#include <ctype.h>

/**
* Client code
* 1. Create a socket and connect to the server specified in the command arugments.
* 2. Prompt the user for input and send that input as a message to the server.
* 3. Print the message received from the server and exit the program.
*/

// Error function used for reporting issues
void error(const char *msg) { 
  perror(msg); 
  exit(0); 
} 

// Set up the address struct
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber, 
                        char* hostname){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);

  // Get the DNS entry for this host name
  struct hostent* hostInfo = gethostbyname(hostname); 
  if (hostInfo == NULL) { 
    fprintf(stderr, "CLIENT: ERROR, no such host\n"); 
    exit(0); 
  }
  // Copy the first IP address from the DNS entry to sin_addr.s_addr
  memcpy((char*) &address->sin_addr.s_addr, 
        hostInfo->h_addr_list[0],
        hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, portNumber, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  //char buffer[256];
  // Check usage & args
  /*
  If number of command line arguments is less than 3
  */
  if (argc < 3) { 
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); 
    exit(0); 
  } 

  // Create a socket
  socketFD = socket(AF_INET, SOCK_STREAM, 0); 
  if (socketFD < 0){
    error("CLIENT: ERROR opening socket");
  }

   // Set up the server address struct
  setupAddressStruct(&serverAddress, atoi(argv[3]), "localhost");

  // Connect to server
  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0){
    error("CLIENT: ERROR connecting");
  }
/*Handshake initiation of processes*/
  char function = 'E';
  int charWritten = send(socketFD, &function, 1,0);
  if (charsWritten < 0){
      error("Clinet writing on the socket");
    }
  
/*
setting up the opening of the files for reading only  and checking they have been opened
*/
FILE *keycontents=fopen(argv[2], "r");
if (keycontents == NULL){
perror( "Error opening key file\n");
return(1);
}

FILE *plaintext = fopen(argv[1],"r");
if (plaintext == NULL){
perror("Error opening plain textfile\n");
return(1);
}

/*Generally used to reead the file contents from the end to the start since u need the size of the file 
and the u have to start from the beginning using seek_set otherwise no data  is read.seek_set u read from the beginnning, and seek_end you
read from end better to read from beginning.
*/
fseek(plaintext, 0L, SEEK_END);
int plaintextsize = ftell(plaintext);
fseek(plaintext, 0L, SEEK_SET);\
char plaintextSizestr[4];
sprintf(plaintextSizestr, "%03d", plaintextsize);

 fseek(keycontents, 0L, SEEK_END);
 int keysize = ftell(keycontents);
 fseek(keycontents, 0L, SEEK_SET);
 char keySizestr[6];
 sprintf(keySizestr, "%05d", keysize);




  // creation of buffers and placing them in arrays and placing them to enable movement of data via the created processes.
  char plaintextchars[1024];
  //char realkey[1024];
  char plaitextlength[256];
  
// try and read the contents from plaintext and read them into the string buffers  so that it is kept and transfered there.
// reading of strings from files and placing them in buffer arrays.
// this initiated the buffer information transfer of data.
  fgets(plaintextchars, sizeof(plaintextchars), plaintext);
  // introducing reading of  the key file as large as the plaintext to avoid transfering data in chunks.
  int plaintext_size = strlen(plaintextchars);
  char* realkey = malloc((plaintext_size + 1)* sizeof(char));
  if(realkey == NULL){
    fprintf(stderr,"Memory dynamic allocation failes and exploded to a dynamite\n");
    exit(1);
  }
  // read from keycontents  and place in realkey but only according to the size of plaintextsize.
 fgets(realkey, plaintext_size + 1, keycontents);
  realkey[plaintext_size] = '\0';
  //checking if the characters are plaintext normal character were looking for like capital letter and spaces.
  for(int i = 0; i < plaintextchars[i] != '\0';i++){
  if (!isalpha(plaintextchars[i]) && plaintextchars[i] != ' '){
      fprintf(stderr,"wrong characters in plaintextfile\n");
      exit(1);
    }
  }
for(int i=0; i < realkey[i] !='\0' ;i++){
if (!isalpha(realkey[i]) && realkey[i] != ' '){
      fprintf(stderr,"wrong chars in file\n");
      exit(1);
    }
  }
  // checking to see if the key given to us is less than the length of the plaintext.
if (strlen(realkey) < strlen(plaintextchars)){
      error("Error char is too short.\n");
      exit(1);
    }
  /*
  initiating data transerf protocol which means start sending the size of plaintext to the server.
  */
  int lengthsent=0;
  while(lengthsent < strlen(plaintextSizestr)){
  charsWritten = send(socketFD, plaintextSizestr + lengthsent, strlen(plaintextSizestr)-lengthsent, 0); 
  if (charsWritten < 0){
    printf("CLIENT: ERROR writing to socket");
  }
  lengthsent += charsWritten;
}
  if (charsWritten < strlen(plaintextSizestr)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }


//initiating data transfer protoocal which means send the actual plaintext to server
 int TotalSent = 0;
 // introducing new variable so that we keep on sending till we have sent all the data.It will continue as long as the chars sent are less than toatal chars to send.
while(TotalSent < plaintextsize){
  charsWritten = send(socketFD,plaintextchars +TotalSent,plaintextsize- TotalSent,0);
  if (charsWritten < 0){
    printf("CLIENT: Error writing to socket");
  }
  TotalSent += charsWritten;
}
if (TotalSent < plaintextsize){
  printf("CLIENT: WARNING: Not all data writtent to the socket!\n"); 
} 

//printf("Clinet sending plain text chars: \"%s\"\n", plaintextchars);
/*
sendinf of the keysize to the server.
*/
  charsWritten = send(socketFD, keySizestr, strlen(keySizestr), 0); 
   //charsWrittens= send(socketFD, realkey, strlen(realkey), 0); 
  if (charsWritten < 0){
    error("CLIENT: ERROR writing to socket");
  }
  if (charsWritten < strlen(keySizestr)){
    printf("CLIENT: WARNING: Not all data written to socket!\n");
  }

// sending the actual key and keeping on sending till all of it is done and has been sent to the server.

int totalSent = 0;
while(totalSent < keysize){
  charsWritten = send(socketFD,realkey +totalSent,keysize- totalSent,0);
  if (charsWritten < 0){
    error("CLIENT: Error writing to socket");
  }
  totalSent += charsWritten;
}
if (totalSent < keysize){
  printf("CLIENT: WARNING: Not all data writtent to the socket!\n"); 
} 
free(realkey);
//printf("Client keys sent: \"%s\"\n", realkey);
fclose(plaintext);
fclose(keycontents);
// create the cipher text buffer be ready to recieve it from the server.print it with its null terminator.
  char ciphertext[1024];
  charsRead = recv(socketFD, ciphertext,  sizeof(ciphertext), 0); 
  if (charsRead < 0){
    error("CLIENT: ERROR reading from socket");
  }
  ciphertext[charsRead] = '\0';
  //printf("%s\n",ciphertext);
  printf("%s\n", ciphertext);

  // Close the socket
  close(socketFD); 
  return 0;
}
