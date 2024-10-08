#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>

// Error function used for reporting issues
void error(const char *msg) {
  perror(msg);
  exit(1);
} 

// Set up the address struct for the server socket
void setupAddressStruct(struct sockaddr_in* address, 
                        int portNumber){
 
  // Clear out the address struct
  memset((char*) address, '\0', sizeof(*address)); 

  // The address should be network capable
  address->sin_family = AF_INET;
  // Store the port number
  address->sin_port = htons(portNumber);
  // Allow a client at any address to connect to this server
  address->sin_addr.s_addr = INADDR_ANY;
}


char lettersofalphabet[27] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};
// /*
// AScii mathematics of mapping letters to numbers and so forth.
// */
/*
Function to conevert letters to numbers.
*/
char lettertonum(char letter){
  if (letter == ' '){
    return 26;
  }
  else if (letter >= 'A' && letter <= 'Z'){
    return letter - 'A'; 
  }
  else if (letter == '\n'){
    return -1;
  }
  else if (letter == '\0'){
    return -1;
  }
  
  else{
    fprintf(stderr, "ERROR: Invalid character found in plaintext: %c (ASCII %d)\n",letter, letter);
    return -1;
  }

}
int number2letter(int number){
  if(number  < 0 || number  > 27){
    fprintf(stderr, "Error invalid number in num to letter function: %d\n",number);
    exit(1);
  }
if (number  == 26){
  return ' ';
}
else {
return number + 'A';
}
}
/*
// doing the opposite but in decrypt where your tryna get back the text
first get the key and the plaintext loop pllaintext since we only vare about the plaintext size
subtract the key from the cipher text
if result is negative add 27
otherwise call our dear number to letter to get back our plaintext size.
*/

char *encrypt(char *plaintext, char *key, int connectionSocket){
  
   int i =0;
  
   
    // Create different cases for the fork  to handle different situation when the fork is released
    // Case -1 is when the fork actually fails.
    
    // Case 0 is when the fork is actually successful and the child process is created.  
    
    int length = strlen(plaintext);
    char *result = malloc(length + 1);
    result[length] = '\0';
    int temp;
  for(int i=0; i < strlen(plaintext); i++){
    int plainnumerals = lettertonum(plaintext[i]);
    int keynumerals = lettertonum(key[i]);
    if(plainnumerals == -1 || keynumerals == -1){
      continue;
    }
    temp = plainnumerals - keynumerals;
    if (temp < 0){
      temp +=27;
    }
    temp = temp % 27;
    result[i] = number2letter(temp);
    }
     return result;
    

   
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  // Check usage & args
  if (argc < 2) { 
    fprintf(stderr,"USAGE: %s port\n", argv[0]); 
    exit(1);
  } 
  
  // Create the socket that will listen for connections
  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) {
    error("ERROR opening socket");
  }

  // Set up the address struct for the server socket
  setupAddressStruct(&serverAddress, atoi(argv[1]));

  // Associate the socket to the port
  if (bind(listenSocket, 
          (struct sockaddr *)&serverAddress, 
          sizeof(serverAddress)) < 0){
    error("ERROR on binding");
  }

  // Start listening for connetions. Allow up to 5 connections to queue up
  listen(listenSocket, 5); 
  
  // Accept a connection, blocking if one is not available until one connects
  while(1){
    // Accept the connection request which creates a connection socket
    connectionSocket = accept(listenSocket, 
                (struct sockaddr *)&clientAddress, 
                &sizeOfClientInfo); 
    if (connectionSocket < 0){
      error("ERROR on accept");
    }
// lets call fork and fork a child process.
    int spawn_pid = fork();

    switch (spawn_pid ) {
      // assuming our fork worked.
 case 0:{
    printf("SERVER: Connected to client running at host %d port %d\n", 
                          ntohs(clientAddress.sin_addr.s_addr),
                          ntohs(clientAddress.sin_port));

    
   char function;
   //recieve the function was it an E or D so that we start our decryption process
    int charsRead = recv(connectionSocket, &function, 1,0);
    if(charsRead < 0){
        error("Error reading from the socket");
    }
    if (function != 'D'){
      close(connectionSocket);
      return 1;
    }

   // Receive plaintext size from client
  char clientelcharsizestr[4];
  memset(clientelcharsizestr, '\0',4);
  /*
  This is where wee are connecting directly to the client through the recv function thats recieving something through the socket that is being sent.
  */
  charsRead = recv(connectionSocket,clientelcharsizestr, sizeof(clientelcharsizestr)-1, 0);
  /*
  check to see if you actually got something from the client.
  */
 if (charsRead < 0){
  error("Error reading from the socket");
  //meaning u got nara.
 }
 /*
 Remember how you made plain text a string.Now first convert it to an integer so it knows what to expect.
 */
int clientelsize= atoi(clientelcharsizestr);
/*
Next step is to check if it has been converted.
*/
if (clientelsize == 0 && clientelcharsizestr[0] != '0'){
  error("Error converting integer");
}
/*
allocating of memory for the plaintext and using a pointer to point where it will staart from.
*/
char *plaintext = malloc(clientelsize + 2);
if (plaintext == NULL){
  error("error allocating memory for plaintext");
}
// use the recv to start getting back the plaintext from client
//clear out the plaintext buffer
memset(plaintext, '\0', clientelsize + 2);
// keep on recieving chars until all the chars have been recieved if they are some missing try to get them back
int totalcharsread =0;
while(totalcharsread < clientelsize){
charsRead = recv(connectionSocket, plaintext+totalcharsread , clientelsize - totalcharsread , 0);
//printf("SERVER Chara : %s\n", plaintext);
if (charsRead < 0){
  error("Error reading from socket");
}
totalcharsread += charsRead;
}

//printf("SERVER: I received plaintext of size: %lu\n", strlen(plaintext));
//printf("SERVER: I received this from the client: \"%s\"\n", plaintext);

char keysizestr[6];
memset(keysizestr, '\0', 6);
charsRead = recv(connectionSocket, keysizestr, 5,0);
// meaning no keysize string was sent over from the client.
if (charsRead < 0){
  error("Error reading from socket");
}
// if u found it do urself a favor and conver it to an integer.This will help in knowing how many number of keys to expect.
//printf("Server recieved keysizestr: \"%s\"\n", keysizestr);
int keysize = atoi(keysizestr);
if (keysize ==0 && keysizestr[0] != '0'){
  error("Error converting integer");
}
// otherwise if keysize was sent chances are that were gonne a need  to allocate memory for our keys.I think dynamically.
char *actualkeys= malloc(keysize + 1);
// checking if memory was allocated for our keys.
if (actualkeys == NULL){
  error("error allocating memory for the keys ");
}
memset(actualkeys, '\0', keysize + 1);
totalcharsread = 0;
while (totalcharsread < keysize){
charsRead = recv(connectionSocket, actualkeys + totalcharsread,keysize - totalcharsread,0);
// checki if any keys were recieved from our connection.
if (charsRead < 0){
  error("Error reading information from the socket");
}
totalcharsread += charsRead;
}
//printf("SERVER I RECIEVED THIS FROM THE CLIENT: \"%s\"\n", actualkeys);
// call encrypt using keys and ciphertext.
char *ciphertext = encrypt(plaintext, actualkeys, connectionSocket);
printf("\"%s\"\n",ciphertext);


free(plaintext);
free(actualkeys);
    // Send a Success message back to the client
    charsRead = send(connectionSocket, ciphertext, strlen(ciphertext), 0); 
    if (charsRead < 0){
      error("ERROR writing to socket");
    }
    // Close the connection socket for this client
    close(connectionSocket); 
    return 0;
    break;
  }
  default:{
    close(connectionSocket);
    //  return 0;
    }
  }
  }

  // Close the listening socket
  close(listenSocket); 
  return 0;
}
