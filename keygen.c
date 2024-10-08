#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
/*
create the char array
create the main function pass in arguments and number of arguments.
pass in the seed your trying to create and use
pass in the length of the key your creating
loop the length of the key 
Pass in the id and random number and then mod it by 27.
add and transform that key to the letter.
prin your key on a newline
*/
/*
This program generates a key of a specified length.

1. The 'keylength' argument is passed in, which specifies the length of the key to be created.
2. A loop runs for 'keylength' iterations to generate each character of the key.
3. In each iteration, a random number is generated and then modulo 27 operation is performed on it. This ensures the result is within the range of 0-26.
4. The resulting number is added to the ASCII value of 'A' to transform it into a letter. This forms the next character of the key.
5. The generated key is printed on a newline.
6. The last character of the output should be a newline.
7. Any error text must be output to stderr.

The syntax for running the program is: keygen keylength
Here 'keylength' is the length of the key file in characters. The program outputs to stdout.
*/
char lettersofalphabet[27] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z',' '};
//argc is arguments and 1st argument passed to it. ** is the all the command line arguments.
/*argv[0] is program name.
argv[1] is first argument.
argv[2] is second argument.
if it has the address it has the value *num is pointer to the value of num ** is pointer to pointer of the value of num.Pointers
deal with addresses.Messing with pointers means messing with addreses meaning high desire for garbage values and seg faults.
One's for the start ones for the specific character.
*/
int main( int argc, char**argv){
    // conversion of the string into a number in the ASCII table.
    int keylength = atoi(argv[1]);
    int length = keylength;
     time_t t;
     // set the see to produce different random numbers with an unsigned integer value according to the time since time changes.
     srand((unsigned) time(&t));
     //allocate memory for length to hold the generate string
     char *keysize = malloc(sizeof(char) * (length + 2));
     for(int i =0; i < length; i++){
        // generate a random number based off our mod 27.
        int random = rand() % 27;
        keysize[i] = lettersofalphabet[random];

     }
     keysize[length] = '\n';
     keysize[length + 1] = '\0';
     printf("%s", keysize);
     free (keysize);
     return 0;



    
}
