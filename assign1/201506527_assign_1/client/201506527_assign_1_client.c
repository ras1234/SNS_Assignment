#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/sha.h>


/* Global constants */
#define SERVICE_PORT 41041
#define MAX_SIZE 20
#define MAX_LEN 1024
#define SUCCESS 1
#define FAIL 0
#define STACK_SIZE 10000 
#define NOT_EXIST 0xFFFF; 
#define LARGE 9988

#define MAX_ITERATION 10 // Max tests in Miller-Robin Primality Test. 
#define div /

#define mod % 
#define and && 
#define true 1 
#define false 0


#define DEFAULT_SERVER "192.168.1.241"

#define REQ 10  /* Request message */
#define REP 20  /* Reply message */
int mul_inverse=0; 
int gcd_value; 

int print_flag=0; 
int print_flag1=0;

typedef struct
{ 
  int top;
  char c[STACK_SIZE];
} stack;

stack s;

typedef short boolean;

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

/* Define a message structure */
typedef struct {
 int opcode;
 int src_addr;
 int dest_addr;
 } Hdr;

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

// // RSA public key 
// typedef struct {
// long e;  //encryption exponent 
// long n; //modulus 
// } PubKey;

boolean verify_prime(long int p)
{
  long int d; // Test for p;
  for(d = 2; d <= (long int) sqrt(p); d++ ) 
	  if ( p % d == 0 ) 
      return false;
  return true;
}

int gcd(int a, int b)
{
  int r;
  if(a < 0) 
    a = -a; 
  if(b < 0) 
    b = -b;
  if(b == 0) 
    return a;
  r = a mod b;
  // exhange r and b, initialize a = b and b = r; 
  a = b; 
  b = r;
  return gcd(a,b);
}

void extended_euclid(int A1, int A2, int A3, int B1, int B2,int B3)
{
  int Q;
  int T1,T2,T3;
  if(B3 == 0)
  {
    gcd_value = A3; 
    mul_inverse = NOT_EXIST; 
    return;
  }
  if(B3 == 1)
  {
    gcd_value = B3; 
    mul_inverse = B2; 
    return;
  }  
  Q = (int)(A3/B3);
  T1 = A1 - Q*B1;
  T2 = A2 - Q*B2;
  T3 = A3 - Q*B3;
  A1 = B1;
  A2 = B2;
  A3 = B3;
  B1 = T1;
  B2 = T2;
  B3 = T3;  
  extended_euclid(A1,A2,A3,B1,B2,B3);
}

boolean MillerRobinTest(long int n, int iteration)
{

//1/ n is the given integer and k is the given desired 

//2/ number of iterations in this primality test algorithm. 

//3/ Return true if all the iterations test passed to give 

//4/ the higher confidence that n is a prime, otherwise 

//5/ return false if n is composite. 

  long int m, t; 
  int i,j;
  long int a, u; 
  int flag;
  if(n mod 2 == 0)
	  return false;  // n is composite.
  m = (n-1) div 2; 
  t = 1;
  while( m mod 2 == 0)  // repeat until m is even
  {
    m = m div 2; 
    t = t + 1;
  }
  for (j= 0; j < iteration; j++) 
  { // Repeat the test for MAX_ITERATION times flag = 0;
    srand((unsigned int) time(NULL));
    a = random() mod n + 1; // select a in {1,2,......,n} u = ModPower(a,m,n);
  if(u == 1 || u == n - 1) 
    flag = 1;
  for(i=0;i<t;i++)
  {
    if(u == n - 1) 
      flag = 1;
    u = (u * u) mod n;
  }
  if ( flag == 0 )
    return false; // n is composite
  }
  return true; // n is prime.
} // end of MillerRobinTest().

void KeyGeneration(key *pub_key, key *pvt_key)
{
  long int p,q; 
  long int n; 
  long int phi_n; 
  long int e;
  // Select p and q which are primes and p<q.
  if(print_flag1)
    printf("\n selecting p->\n\r");
 while(1)
 {
  while(1)
  {
    srand((unsigned int) time(NULL)); 
    p = random() % LARGE;
    /* test for even number */
    if ( p & 0x01 == 0 ) 
      continue;
    if(MillerRobinTest(p, MAX_ITERATION)) 
      break;
  }
  if(print_flag1)
    printf("\n selecting q->\n\r");
  while(1)
  {
    srand((unsigned int) time(NULL)); 
    q=random() % LARGE;
    if( q == p)
    {
      srand((unsigned int) time(NULL)); 
      q = random() % LARGE;
      continue;
    }
    if(MillerRobinTest(q, MAX_ITERATION)) 
      break;
  }
  // Compute n.
  if (verify_prime(p) && verify_prime(q) ) 
  {
    printf("p = %ld, q = %ld are primes\n", p, q);
    break;
  }
  else 
  {
    //printf("p = %ld, q = %ld are composite\n", p, q);
  }
  //printf( "p = %ld, q = %ld\n", p, q); 
  n = p * q;
  //printf("%ld\n",n);
  // Compute Euler's phi(totient) function 
  phi_n = (p-1)*(q-1);
  // Compute e such that gcd(e,phi_n(n))=1.
  if(print_flag1)
    printf("\n selcting e->\n\r");
  while(1)
  {
    e = random()%phi_n; 
    if(gcd(e, phi_n)==1)
      break;
  }
  // Compute d such that ed=1(mod phi_n(n)).
  if(print_flag1)
    printf("\n selceting d-> \n\r"); 
  extended_euclid(1, 0, phi_n, 0, 1, e);
  if(mul_inverse <0) 
  {
    mul_inverse = - mul_inverse;
    mul_inverse = ((phi_n - 1 ) * mul_inverse) mod phi_n;
  }
  if(print_flag1)
    printf("\n phi_n= %ld\n\n",phi_n);
  //1/ Put Public Key and Private Key. 
  pub_key->public_key.n = n; 
  pub_key->public_key.e = e; 
  pvt_key->private_key.n = n; 
  pvt_key->private_key.d = mul_inverse;
} 
}// end of KeyGeneraion()

/* Function prototypes */
int serverConnect ( char * );
void Talk_to_server ( int );

/* Connect with the server: socket() and connect() */
int serverConnect ( char *sip )
{
   int cfd;
   struct sockaddr_in saddr;   /* address of server */
   int status;

   /* request for a socket descriptor */
   cfd = socket (AF_INET, SOCK_STREAM, 0);
   if (cfd == -1) {
      fprintf (stderr, "*** Client error: unable to get socket descriptor\n");
      exit(1);
   }
   /* set server address */
   saddr.sin_family = AF_INET;              /* Default value for most applications */
   saddr.sin_port = htons(SERVICE_PORT);    /* Service port in network byte order */
   saddr.sin_addr.s_addr = inet_addr(sip);  /* Convert server's IP to short int */
   bzero(&(saddr.sin_zero),8);              /* zero the rest of the structure */

   /* set up connection with the server */
   status = connect(cfd, (struct sockaddr *)&saddr, sizeof(struct sockaddr));
   if (status == -1) {
      fprintf(stderr, "*** Client error: unable to connect to server\n");
      exit(1);
   }
   fprintf(stderr, "Connected to server\n");
   return cfd;
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
} // end of ModPowe

long int DecryptionAlgorithm(long int C, key pvt_key)
{
  //Bob retrieves M as M := C^d(mod n) 
  long int M;
  if(print_flag1)
    printf( "\n Decryption keys= ( %ld,%ld)\n\r",pvt_key.private_key.n,pvt_key.private_key.d); 
  M=ModPower(C, pvt_key.private_key.d, pvt_key.private_key.n);
  return M;
}

/* Interaction with the server */
void Talk_to_server ( int cfd )
{
  char buffer[MAX_LEN];
  int nbytes, status;
  int src_addr, dest_addr;
  ReqMsg send_msg;
  RepMsg recv_msg;
	key pub_key, pvt_key;
  long int deciphertext,ciphertext;

	printf("\nKey generation has been started by Client:\n\r"); 
  KeyGeneration(&pub_key, &pvt_key);
  printf("\n Public Key of Alice is (n,e): (%ld , %ld)\n\r", pub_key.public_key.n, pub_key.public_key.e);
  printf("\n Private key of Alice is (n,d): (%ld , %ld)\n\r", pvt_key.private_key.n, pvt_key.private_key.d);
  printf("%s\n",send_msg.file);
  int i;
  for(i=0;i<recv_msg.filelength;i++)
    printf("%d ",recv_msg.fileinfo[i]);

   dest_addr = inet_addr("DEFAULT_SERVER");
   src_addr = inet_addr("192.168.1.245");

   /* send the request message REQ to the server */
   printf("Sending the request message REQ to the server\n");          
   send_msg.hdr.opcode = REQ;
   send_msg.hdr.src_addr = src_addr;
   send_msg.hdr.dest_addr = dest_addr;
   
   srand((unsigned int) time(NULL));
	 send_msg.x=pub_key.public_key.n;
	 send_msg.y=pub_key.public_key.e;
   //char f[1000]="sample.txt";
   strcpy(send_msg.file,"sample.txt");
   status = send(cfd, &send_msg, sizeof(ReqMsg), 0);
   if (status == -1) 
   {
      fprintf(stderr, "*** Server error: unable to send\n");
      return;
   }

  while (1) {
  /* receive greetings from server */
   nbytes = recv(cfd, &recv_msg, sizeof(RepMsg), 0);
   if (nbytes == -1) {
      fprintf(stderr, "*** Client error: unable to receive\n");
      
   }
   switch ( recv_msg.hdr.opcode ) {
    
   case REP : printf("Message:: with opcode %d (REP) received from source (%d)\n", recv_msg.hdr.opcode, recv_msg.hdr.src_addr);  
              // printf("Received Encoded File :");
              int k;
              // for(k=0;k<recv_msg.filelength;k++)
              //   printf("%d ",recv_msg.fileinfo[k]);
              // printf("\n");
             // printf("cipherfile data is as follow\n");
              // long int l;
              // for(l=0;l<recv_msg.cipherlength;l++)
              //   printf("%ld ",recv_msg.cipherfile[l]);
              // printf("\n");
              int t;
              int decodefile[500];
              for(t=0;t<recv_msg.cipherlength;t++)
              {
                ciphertext=recv_msg.cipherfile[t];
                deciphertext = DecryptionAlgorithm(ciphertext,pvt_key);
                decodefile[t]=deciphertext;
                //printf("\n\r The decipher text (recovered original plaintext) produced by Alice is : %ld\n\r", deciphertext);
              }
              int m;
              // for(m=0;m<recv_msg.cipherlength;m++)
              //   printf("%d ",decodefile[m]);
              // printf("\n");  
              char hash[64];
              hash[0]=' ';
              hash[61]=',';
              hash[62]='.';
              hash[63]='!';
              int i;
              int upper=65;
              for(i=1;i<=26;i++)
              {
                hash[i]=upper;
                upper++;
              }
              int lower=97;
              for(i=27;i<=52;i++)
              {
                hash[i]=lower;
                lower++;
              }
              int invalue=0;
              for(i=53;i<=60;i++)
              {
                hash[i]=invalue+'0';
                invalue++;
              }
               // for(i=0;i<64;i++)
               //    printf(" %d ---%c \n",i,hash[i]);
              char storedecodefile[5000];
              for(t=0;t<recv_msg.cipherlength;t++)
              {
                  if(decodefile[t]==63)
                    storedecodefile[t]=hash[63];
                  else if(decodefile[t]==62)
                    storedecodefile[t]=hash[62];
                  else if(decodefile[t]==61)
                    storedecodefile[t]=hash[61];
                  else if(decodefile[t]==0)
                    storedecodefile[t]=hash[0];
                  else if(decodefile[t]>=1 && decodefile[t]<=60)
                    storedecodefile[t]=hash[decodefile[t]];
              }
              int d;
              // for(d=0;d<recv_msg.cipherlength;d++)
              //   printf("%c",storedecodefile[d]);
              FILE *fp;
              fp=fopen("sample.txt","a");
              for(d=0;d<recv_msg.cipherlength;d++)
              {
                 fprintf(fp, "%c",storedecodefile[d]);
              }
              fclose(fp);
              // for(d=0;d<recv_msg.cipherlength;d++)
              //   printf("%d---%c\n",d,storedecodefile[d]);
              // printf("length=%ld\n",recv_msg.cipherlength);
              int shalen=recv_msg.shalength;
              char temp7[5000];
              for(d=0;d<shalen;d++)
                  temp7[d]=storedecodefile[d];
              int x = 0;
              unsigned char temp[SHA_DIGEST_LENGTH];
              char buf[SHA_DIGEST_LENGTH*2];
              memset(buf, 0x0, SHA_DIGEST_LENGTH*2);
              memset(temp, 0x0, SHA_DIGEST_LENGTH);
              SHA1((unsigned char *)temp7,strlen(temp7), temp);
              for (x=0; x < SHA_DIGEST_LENGTH; x++) 
              {
                  sprintf((char*)&(buf[x*2]), "%02x", temp[x]);
              }
              //storedecodefile[count]='\0';
              // printf("SHA1 is %s\n",buf);
              // printf("received sha1 is %s\n",recv_msg.serverbuf);


              /* Check the status of REP message */
              if (recv_msg.status) 
                printf("\nMessage REQ has received successfully by the server\n");
              else    
               printf("\nMessage REQ has NOT received successfully by the server\n");
             printf("\n");
              break;
   default: 
           printf("message received with opcode: %d\n", recv_msg.hdr.opcode);
           exit(0);  
   }
 }
}

int main ( int argc, char *argv[] )
{
   ReqMsg send_msg;
   char sip[16];
   int cfd;
   //send_msg.file=argv[1];
   printf("******* This is demo program using sockets***** \n\n");
   printf("%d",argc);
   strcpy(sip, (argc == 2) ? argv[1] : DEFAULT_SERVER);
   cfd = serverConnect(sip);
   Talk_to_server (cfd);
	
   close(cfd);
}

/*** End of client.c ***/
