#include <arpa/inet.h>
#include <stdio.h>
#include<string.h>
#include<math.h>
#include<stdlib.h>
#include<time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <unistd.h>
#include <openssl/sha.h>

/* Global constants */
#define SERVICE_PORT 41041
#define MAX_SIZE 20
#define Q_SIZE 5

#define SUCCESS 1
#define FAIL 0

#define DEFAULT_SERVER "192.168.1.241"

#define REQ 10  /* Request message */
#define REP 20  /* Reply message */

#define STACK_SIZE 10000 
#define NOT_EXIST 0xFFFF; 
#define LARGE 99

#define MAX_ITERATION 10 // Max tests in Miller-Robin Primality Test. 
#define div /

#define mod % 
#define and && 
#define true 1 
#define false 0


typedef struct{ int top;

char c[STACK_SIZE];

} stack;

typedef short boolean;

/* Define a message structure */
typedef struct {
 int opcode;
 int src_addr;
 int dest_addr;
 } Hdr;

typedef union{ 
  struct{
    long int n; 
    long int e;
  } public_key;

  struct{
    long int n; 
    long int d;
  } private_key;
} key;

/* REQ message */
typedef struct {
 Hdr hdr;
 int x;
 int y;
 char file[5000];
 //int check1; /* x AND y */
 //int check2; /* x XOR y */
} ReqMsg;

/* REP message */
typedef struct {
 Hdr hdr;
 int status;
 int fileinfo[5000];
 int filelength;
 long int cipherfile[5000];
 long int cipherlength;
 char serverbuf[SHA_DIGEST_LENGTH*2];
 int shalength;
} RepMsg;

int mul_inverse=0; 
int gcd_value; stack s;

int print_flag=0; 
int print_flag1=0;

/* Function prototypes */
int startServer ( );
void Talk_to_client ( int );
void serverLoop ( int );

/* Start the server: socket(), bind() and listen() */
int startServer ()
{
   int sfd;                    /* for listening to port PORT_NUMBER */
   struct sockaddr_in saddr;   /* address of server */
   int status;


   /* Request for a socket descriptor */
   sfd = socket(AF_INET, SOCK_STREAM, 0);
   if (sfd == -1) {
      fprintf(stderr, "*** Server error: unable to get socket descriptor\n");
      exit(1);
   }

   /* Set the fields of server's internet address structure */
   saddr.sin_family = AF_INET;            /* Default value for most applications */
   saddr.sin_port = htons(SERVICE_PORT);  /* Service port in network byte order */
   saddr.sin_addr.s_addr = INADDR_ANY;    /* Server's local address: 0.0.0.0 (htons not necessary) */
   bzero(&(saddr.sin_zero),8);            /* zero the rest of the structure */

   /* Bind the socket to SERVICE_PORT for listening */
   status = bind(sfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Server error: unable to bind to port %d\n", SERVICE_PORT);
      exit(2);
   }

   /* Now listen to the service port */
   status = listen(sfd,Q_SIZE);
   if (status == -1) {
      fprintf(stderr, "*** Server error: unable to listen\n");
      exit(3);
   }

   fprintf(stderr, "+++ Server successfully started, listening to port %hd\n", SERVICE_PORT);
   return sfd;
}


/* Accept connections from clients, spawn a child process for each request */
void serverLoop ( int sfd )
{
   int cfd;                    /* for communication with clients */
   struct sockaddr_in caddr;   /* address of client */
   int size;


    while (1) {
      /* accept connection from clients */
      cfd = accept(sfd, (struct sockaddr *)&caddr, &size);
      if (cfd == -1) {
         fprintf(stderr, "*** Server error: unable to accept request\n");
         continue;
      }

     printf("**** Connected with %s\n", inet_ntoa(caddr.sin_addr));
     
      /* fork a child to process request from client */
      if (!fork()) {
         Talk_to_client (cfd);
         fprintf(stderr, "**** Closed connection with %s\n", inet_ntoa(caddr.sin_addr));
         close(cfd);
         exit(0);
      }

      /* parent (server) does not talk with clients */
      close(cfd);

      /* parent waits for termination of child processes */
      while (waitpid(-1,NULL,WNOHANG) > 0);
   }
}

void decimal_to_binary(long int n,char str[])
{
  //1 n is the given decimal integer. 
  //2/ Purpose is to find the binary conversion 
  //3/ of n. 
  //4/ Initialise the stack. 
  int r; 
  s.top = 0;
  while(n != 0)
  {
    r = n mod 2; 
    s.top++;
    if(s.top >= STACK_SIZE)
    {
      printf("\nstack overflown!\n"); 
      return;
    }
    s.c[s.top] = r + 48; 
    if(print_flag)
      printf("\n s.c[%d]= %c\n", s.top, s.c[s.top]); 
    n = n div 2;
  }
  while(s.top)
  {
    *str++ = s.c[s.top--];
  }
  *str='\0';
  return;
}


// Algorithm: reverse a string.

void reverse_string(char x[])

{

int n = strlen(x)-1; int i = 0;

char temp[STACK_SIZE];

for(i = 0; i<=n; i++) temp[i] = x[n-i];

for(i=0; i<=n; i++) x[i] = temp[i];

}

// Algorithm: Modular Power: x^e(mod n).
long int ModPower(long int x, long int e, long int n)
{
  //To calculate y:=x^e(mod n). 
  //long y;
  long int y; 
  long int t; 
  int i;
  int BitLength_e; 
  char b[STACK_SIZE];
  //printf("e(decimal) = %ld\n",e); 
  decimal_to_binary(e,b); 
  if(print_flag)
    printf("b = %s\n", b); 
  BitLength_e = strlen(b); 
  y = x; 
  reverse_string(b);
  for(i = BitLength_e - 2; i >= 0 ; i--)
  {
    if(print_flag) 
      printf("\nb[%d]=%c", i, b[i]);
    if(b[i] == '0') 
      t = 1;
    else t = x;
    y = (y * y) mod n; 
    if ( y < 0 ) 
    {
      y = -y;
      y = (y - 1) * (y mod n) mod n; 
      printf("y is negative\n");
    }
    y = (y*t) mod n; 
    if ( y < 0 ) 
    {
      y = -y;
      y = (y - 1) * (y mod n) mod n; 
      printf("y is negative\n");
    }
  }
  if ( y < 0 ) 
  { 
    y = -y;
    y = (y - 1) * (y mod n) mod n; 
    printf("y is negative\n");
  }
  return y;
} // end of ModPower().

long int EncryptionAlgorithm(long int M,long int publickey,long int privatekey)
{
// Alice computes ciphertext as C := M^e(mod n) to Bob. 
long int C;
if(print_flag1)
printf( "\n Encryption keys= ( %ld,%ld)\n\r",publickey,privatekey); 
C = ModPower(M, privatekey, publickey);
return C;
}

/* Interaction of the child process with the client */
void Talk_to_client (int cfd)
{
   int status;
   int nbytes;
   int src_addr, dest_addr;
   int chk1, chk2; 
   RepMsg send_msg;
   ReqMsg recv_msg;
   long int plaintext,ciphertext;
   key pub_key, pvt_key;
   //key pub_key, pvt_key;

   dest_addr = inet_addr("192.168.1.245");
   src_addr = inet_addr("DEFAULT_SERVER");
 
   while (1) {
   /* Receive response from server */
   nbytes = recv(cfd, &recv_msg, sizeof(ReqMsg), 0);
   if (nbytes == -1) {
      fprintf(stderr, "*** Server error: unable to receive\n");
      return;
   }
   switch ( recv_msg.hdr.opcode ) {
    
   case REQ : /* Request message */
              printf("Message:: with opcode %d (REQ) received from source (%d)\n", recv_msg.hdr.opcode, recv_msg.hdr.src_addr);  
              send_msg.hdr.opcode = REP;
              send_msg.hdr.src_addr = src_addr;        
              send_msg.hdr.dest_addr = dest_addr;  
              printf("Received values in REQ message are: \n");
              printf("n = %d\n", recv_msg.x);
              printf("e = %d\n", recv_msg.y);             
              printf("filename is %s\n",recv_msg.file);
              FILE *fp;
              char ch;
              fp = fopen("sample.txt","r");
              if( fp == NULL )
              {
                printf("Error while opening the file.\n");
                exit(EXIT_FAILURE);
              } 
              int encodefile[5000];
              int hash[64];
              int ind=0;
              int i;
              for(i=0;i<64;i++)
                hash[i]=i;
              // for(int i=0;i<64;i++)
              // printf("%d ",hash[i]);
              int count=0;
              while((ch=fgetc(fp))!=EOF)
              {
                count++;
              }
            //  printf("count value is %d\n",count);
              fclose(fp);
              fp=fopen("sample.txt","r");
              int blocksize=50;
              // printf("count=%d\n",count );
              if(count<=100)
              { 
                 char tempfile[count];
                 int ind=0;
                while((ch=fgetc(fp))!=EOF)
                {
                  tempfile[ind]=ch;
                if(ch>=65 && ch<=90)
                {
                  encodefile[ind]=hash[ch-'A'+1];
                }
                else if(ch>=97 && ch<=122)
                {
                  encodefile[ind]=hash[ch-'a'+27];
                }
                else if(ch==32)
                {
                  encodefile[ind]=hash[0];
                }
                else if(ch>=48 && ch>=57)
                {
                  encodefile[ind]=hash[100-ch];
                } 
                else if(ch==44)
                {
                  encodefile[ind]=hash[61];
                }
                else if(ch==46)
                {
                  encodefile[ind]=hash[62];
                }
                else if(ch==33)
                {
                  encodefile[ind]=hash[63];
                }
                ind++;
                if(ind==count)
                  break;
                }   
                  send_msg.filelength=count;
                  for(i=0;i<count;i++)
                  {
                    send_msg.fileinfo[i]=encodefile[i];
                    //printf("%d ",send_msg.fileinfo[i] );
                  }
                 // printf("\n");
                  int k;
                  for(k=0;k<count;k++)
                  {
                    plaintext=send_msg.fileinfo[k];
                    ciphertext = EncryptionAlgorithm(plaintext,recv_msg.x,recv_msg.y);
                    send_msg.cipherfile[k]=ciphertext;
                   // printf("\n\r The ciphertext produced by Bob is : %ld \n\r", ciphertext);
                  }
                  send_msg.cipherlength=(long int)(count-1);
                  //printf("cipherlength sent is %ld\n",send_msg.cipherlength );
                  // int l;
                  // for(l=0;l<count;l++)
                  // {
                  //   printf("%ld ",send_msg.cipherfile[l]);
                  // }
                  // printf("\n");
                  // int l1=sizeof(tempfile);
                  // printf("templength=%d\n",l1);

                    // int d;
                    // for(d=0;d<count-1;d++)
                    //   printf(" %d---%c\n",d,tempfile[d]);
                    //printf("tempfile len=%d\n",send_msg.filelength);
                  //  char newfile[count-1];
                  //  for(d=0;d<l1-1;d++)
                  //     newfile[d]=tempfile[d];
                  //    int l2=sizeof(newfile);
                  // printf("newfile length=%d\n",l2);
                  //   int ln=strlen(newfile);
                    // printf("length of newfile is %d\n",ln );
                    // printf("file length is=%d\n",count-1);
                    // printf("%d\n",count);
              // int x = 0;
              // unsigned char temp[SHA_DIGEST_LENGTH];
              // memset(send_msg.serverbuf, 0x0, SHA_DIGEST_LENGTH*2);
              // memset(temp, 0x0, SHA_DIGEST_LENGTH);
              // SHA1((unsigned char *)tempfile,strlen(tempfile), temp);
              // for (x=0; x < SHA_DIGEST_LENGTH; x++) 
              // {
              //     sprintf((char*)&(send_msg.serverbuf[x*2]), "%02x", temp[x]);
              // }
              // //newfile[count]='\0';
              // printf("SHA1 of %s is %s\n",tempfile,send_msg.serverbuf);


              //     //if ( (chk1 != recv_msg.check1) && (chk2 != recv_msg.check2) )
                  // send_msg.status = FAIL;
                  //else
                  send_msg.status = SUCCESS;
                  /* send the reply message REP to the server */
                  printf("Sending the reply message REP to the client \n"); 
                  status = send(cfd, &send_msg, sizeof(RepMsg), 0);
                  if(status == -1) 
                  {
                    fprintf(stderr, "*** Client error: unable to send\n"); 
                    for(i=0;i<ind-1;i++) 
                      return;
                  }
                  break;   
              }
              char temp7[5000];
              int tline=0;

              while((ch=fgetc(fp))!=EOF)
              {
                temp7[tline]=ch;
                tline++;
                if(ch>=65 && ch<=90)
                {
                  encodefile[ind]=hash[ch-'A'+1];
                }
                else if(ch>=97 && ch<=122)
                {
                  encodefile[ind]=hash[ch-'a'+27];
                }
                else if(ch==32)
                {
                  encodefile[ind]=hash[0];
                }
                else if(ch>=48 && ch>=57)
                {
                  encodefile[ind]=hash[100-ch];
                } 
                else if(ch==44)
                {
                  encodefile[ind]=hash[61];
                }
                else if(ch==46)
                {
                  encodefile[ind]=hash[62];
                }
                else if(ch==33)
                {
                  encodefile[ind]=hash[63];
                }
                ind++;
                
                if(count>100)
                {
                  if(ind==blocksize)
                  {
                    // memset(fileinfo,0,sizeof(fileinfo));
                    send_msg.filelength=blocksize;
                    for(i=0;i<blocksize;i++)
                    {
                      send_msg.fileinfo[i]=encodefile[i];
                     // printf("%d ",send_msg.fileinfo[i] );
                    }
                    //printf("\n");
                    int k;
                    for(k=0;k<blocksize;k++)
                    {
                      plaintext=send_msg.fileinfo[k];
                      ciphertext = EncryptionAlgorithm(plaintext,recv_msg.x,recv_msg.y);
                      send_msg.cipherfile[k]=ciphertext;
                      //printf("\n\r The ciphertext produced by Bob is : %ld \n\r", ciphertext);
                    }
                    send_msg.cipherlength=(long int)(blocksize);
                   // printf("cipherlength sent is %ld\n",send_msg.cipherlength );
                    // int l;
                    // for(l=0;l<blocksize;l++)
                    // {
                    //   printf("%ld ",send_msg.cipherfile[l]);
                    // }
                    // printf("\n");

              int x = 0;
              unsigned char temp[SHA_DIGEST_LENGTH];
              memset(send_msg.serverbuf, 0x0, SHA_DIGEST_LENGTH*2);
              memset(temp, 0x0, SHA_DIGEST_LENGTH);
              SHA1((unsigned char *)temp7,strlen(temp7), temp);
              for (x=0; x < SHA_DIGEST_LENGTH; x++) 
              {
                  sprintf((char*)&(send_msg.serverbuf[x*2]), "%02x", temp[x]);
              }
              //newfile[count]='\0';
              int shalen=strlen(temp7);
              send_msg.shalength=shalen;
             // printf("SHA1 of %s is %s\n",temp7,send_msg.serverbuf);

                    send_msg.status = SUCCESS;
                    ind=0;
                    // printf("index value is %d\n",ind);
                    send_msg.status = SUCCESS;
                    /* send the reply message REP to the server */
                    printf("Sending the reply message REP to the client \n"); 
                    status = send(cfd, &send_msg, sizeof(RepMsg), 0);
                    if(status == -1) 
                    {
                      fprintf(stderr, "*** Client error: unable to send\n"); 
                      for(i=0;i<ind-1;i++) 
                        return;
                    }
                  }
                  else 
                    continue;
                }
                ind=0;
                
              }
              if(ind>0)
              {
                 // printf("%d\n",ind);
                  send_msg.filelength=ind;
                  for(i=0;i<ind;i++)
                  {
                    send_msg.fileinfo[i]=encodefile[i];
                   // printf("%d ",send_msg.fileinfo[i] );
                  }
                  //printf("\n");
                  int k;
                  for(k=0;k<ind;k++)
                  {
                    plaintext=send_msg.fileinfo[k];
                    ciphertext = EncryptionAlgorithm(plaintext,recv_msg.x,recv_msg.y);
                    send_msg.cipherfile[k]=ciphertext;
                    //printf("\n\r The ciphertext produced by Bob is : %ld \n\r", ciphertext);
                  }
                  send_msg.cipherlength=(long int)(ind);
                  // int l;
                  // for(l=0;l<ind;l++)
                  // {
                  //   printf("%ld ",send_msg.cipherfile[l]);
                  // }
                  // printf("\n");
                  //if ( (chk1 != recv_msg.check1) && (chk2 != recv_msg.check2) )
                  // send_msg.status = FAIL;
                  //else
                 // printf("cipherlength sent is %ld\n",send_msg.cipherlength );
                  send_msg.status = SUCCESS;
                  /* send the reply message REP to the server */
                  printf("Sending the reply message REP to the client \n"); 
                  status = send(cfd, &send_msg, sizeof(RepMsg), 0);
                  if(status == -1) 
                  {
                    fprintf(stderr, "*** Client error: unable to send\n"); 
                    for(i=0;i<ind-1;i++) 
                      return;
                  }
                  break;
              }
              break;
              printf("\n");
    default: 
           printf("message received with opcode: %d\n", recv_msg.hdr.opcode);
           exit(0);  
   }
 }
}

int main ()
{
   int sfd;
	printf("hi\n");
   sfd = startServer();   
   serverLoop(sfd);
}

/*** End of server.c ***/     

