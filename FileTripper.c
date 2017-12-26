#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <time.h>

#define Nc 4			//No of columns for a State Matrix
int Nr=0;				//No of rows for a State Matrix
int Nk=0;				
unsigned char in[16], out[16], state[4][4];
char RoundKey[240];
unsigned char Key[32];
char pass[100];

int generateNumber(){       
    //function to generate a random number between 0 and 9
    int a;
    a=rand()%10;    //modules by 10 of the randomly generated number
    return a;
}

char generateCapitalAlphabet(){
    //function to generate a random capital alphabet
    char p;
    p=rand()%26+65;   //modules by 26 and adding 65 a Capital A ASCII value is 65
    return p;
}

char generateSmallAlphabet(){
    //function to generate a random small alphabet
    char p;
    p=rand()%26+97;    //modules by 26 and adding 97 a Capital a ASCII value is 97
    return p;
}

char generateSpecialCharacter(){
    //generae a special character
    int a=rand()%3+1;
    char p;
    if(a==1){
        p=rand()%14+33; //between 33 and 74
    }
    else if(a==2){
        p=rand()%5+91;  //between 91 and 96
    }
    else{
        p=64;   //@
    }
    return p;
}
int generateLength(){           
    //generate a random length of the password between 8 and 13
    int a=rand()%5+8;
    return a;
}

void generate(){
    int l,i=0,r,j;
    srand( time(0));
    l=generateLength();     //generate the length
    while(i<=l){
        r=rand()%4;
        if(r==1)
        {
            pass[i]=generateNumber()+57;    
        }
        else if(r==2)
        {
            pass[i]=generateSmallAlphabet();
        }
        else if(r==3)
        {
            pass[i]=generateCapitalAlphabet();
        }
        else
        {
            pass[i]=generateSpecialCharacter();
        }
        i++;
    }
}

int getSBoxValue(int num)		//standard table of 256 values usd for subBytes
{
	int sbox[256] =   {
	0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76, 
	0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0, 
	0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15, 
	0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75, 
	0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84, 
	0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf, 
	0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
	0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
	0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
	0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
	0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
	0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
	0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a, 
	0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
	0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
	0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 }; 
	return sbox[num];
}


int getSBoxInvert(int num)		//standard table of256 values for reverse subbytes
{
int rsbox[256] ={
    0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb
  , 0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb
  , 0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e
  , 0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25
  , 0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92
  , 0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84
  , 0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06
  , 0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b
  , 0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73
  , 0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e
  , 0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b
  , 0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4
  , 0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f
  , 0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef
  , 0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61
  , 0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d 
};
return rsbox[num];
}


int Rcon[255] = {			//array for creating the key
	0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 
	0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 
	0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 
	0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 
	0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 
	0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 
	0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 
	0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 
	0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 
	0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 
	0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 0xc6, 0x97, 0x35, 
	0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 0x61, 0xc2, 0x9f, 
	0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb, 0x8d, 0x01, 0x02, 0x04, 
	0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36, 0x6c, 0xd8, 0xab, 0x4d, 0x9a, 0x2f, 0x5e, 0xbc, 0x63, 
	0xc6, 0x97, 0x35, 0x6a, 0xd4, 0xb3, 0x7d, 0xfa, 0xef, 0xc5, 0x91, 0x39, 0x72, 0xe4, 0xd3, 0xbd, 
	0x61, 0xc2, 0x9f, 0x25, 0x4a, 0x94, 0x33, 0x66, 0xcc, 0x83, 0x1d, 0x3a, 0x74, 0xe8, 0xcb  };

void KeyExpansion()
{
	int i,j;
	unsigned char temp[4],k;
	for(i=0;i<Nk;i++)
	{
		RoundKey[i*4]=Key[i*4];
		RoundKey[i*4+1]=Key[i*4+1];
		RoundKey[i*4+2]=Key[i*4+2];
		RoundKey[i*4+3]=Key[i*4+3];
	}
	while (i < (Nc * (Nr+1)))
	{
					for(j=0;j<4;j++)
					{
						temp[j]=RoundKey[(i-1) * 4 + j];
					}
					if (i % Nk == 0)
					{
						{
							k = temp[0];
							temp[0] = temp[1];
							temp[1] = temp[2];
							temp[2] = temp[3];
							temp[3] = k;
						}
						{
							temp[0]=getSBoxValue(temp[0]);
							temp[1]=getSBoxValue(temp[1]);
							temp[2]=getSBoxValue(temp[2]);
							temp[3]=getSBoxValue(temp[3]);
						}

						temp[0] =  temp[0] ^ Rcon[i/Nk];
					}
					else if (Nk > 6 && i % Nk == 4)
					{
						{
							temp[0]=getSBoxValue(temp[0]);
							temp[1]=getSBoxValue(temp[1]);
							temp[2]=getSBoxValue(temp[2]);
							temp[3]=getSBoxValue(temp[3]);
						}
					}
					RoundKey[i*4+0] = RoundKey[(i-Nk)*4+0] ^ temp[0];
					RoundKey[i*4+1] = RoundKey[(i-Nk)*4+1] ^ temp[1];
					RoundKey[i*4+2] = RoundKey[(i-Nk)*4+2] ^ temp[2];
					RoundKey[i*4+3] = RoundKey[(i-Nk)*4+3] ^ temp[3];
					i++;
	}
}

void AddRoundKey(int round) 
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] ^= RoundKey[round * Nc * 4 + i * Nc + j];
		}
	}
}

void SubBytes()
{
	int i,j;
	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[i][j] = getSBoxValue(state[i][j]);

		}
	}
}

void InvSubBytes()
{
    int i,j;
    for(i=0;i<4;i++)
        {
            for(j=0;j<4;j++)
                {
                    state[i][j] = getSBoxInvert(state[i][j]);
                }
        }

}

void ShiftRows()
{
	unsigned char temp;

	// Rotate first row 1 columns to left	
	temp=state[1][0];
	state[1][0]=state[1][1];
	state[1][1]=state[1][2];
	state[1][2]=state[1][3];
	state[1][3]=temp;

	// Rotate second row 2 columns to left	
	temp=state[2][0];
	state[2][0]=state[2][2];
	state[2][2]=temp;

	temp=state[2][1];
	state[2][1]=state[2][3];
	state[2][3]=temp;

	// Rotate third row 3 columns to left
	temp=state[3][0];
	state[3][0]=state[3][3];
	state[3][3]=state[3][2];
	state[3][2]=state[3][1];
	state[3][1]=temp;
}


void InvShiftRows()
{
unsigned char temp;
temp=state[1][3];

state[1][3]=state[1][2];
state[1][2]=state[1][1];
state[1][1]=state[1][0];
state[1][0]=temp;

temp=state[2][0];
state[2][0]=state[2][2];
state[2][2]=temp;

temp=state[2][1];
state[2][1]=state[2][3];
state[2][3]=temp;

temp=state[3][0];
state[3][0]=state[3][1];
state[3][1]=state[3][2];
state[3][2]=state[3][3];
state[3][3]=temp;

}

// xtime is a macro that finds the product of {02} and the argument to xtime modulo {1b}  
#define xtime(x)   ((x<<1) ^ (((x>>7) & 1) * 0x1b))

#define Multiply(x,y) (((y & 1) * x) ^ ((y>>1 & 1) * xtime(x)) ^ ((y>>2 & 1) * xtime(xtime(x))) ^ ((y>>3 & 1) * xtime(xtime(xtime(x)))) ^ ((y>>4 & 1) * xtime(xtime(xtime(xtime(x))))))

// MixColumns function mixes the columns of the state matrix
void MixColumns()
{
	int i;
	unsigned char Tmp,Tm,t;
	for(i=0;i<4;i++)
	{	
		t=state[0][i];
		Tmp = state[0][i] ^ state[1][i] ^ state[2][i] ^ state[3][i] ;
		Tm = state[0][i] ^ state[1][i] ; 
        Tm = xtime(Tm); 
        state[0][i] ^= Tm ^ Tmp ;
		Tm = state[1][i] ^ state[2][i] ; 
        Tm = xtime(Tm); 
        state[1][i] ^= Tm ^ Tmp ;
		Tm = state[2][i] ^ state[3][i] ; 
        Tm = xtime(Tm); 
        state[2][i] ^= Tm ^ Tmp ;
		Tm = state[3][i] ^ t ; 
        Tm = xtime(Tm); 
        state[3][i] ^= Tm ^ Tmp ;
	}
}

void InvMixColumns()
{
int i;
unsigned char a,b,c,d;

    for(i=0;i<4;i++)
    {
        a = state[0][i];
        b = state[1][i];
        c = state[2][i];
        d = state[3][i];

    state[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
    state[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
    state[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
    state[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
    }

}


// Cipher is the main function that encrypts the PlainText.
void Cipher()
{
	int i,j,round=0;

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			state[j][i] = in[i*4 + j];
		}
	}
	AddRoundKey(0); 
	
	for(round=1;round<Nr;round++)
	{
		SubBytes();
		ShiftRows();
		MixColumns();
		AddRoundKey(round);
	}
	
	SubBytes();
	ShiftRows();
	AddRoundKey(Nr);

	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
		{
			out[i*4+j]=state[j][i];
		}
	}
}

void InvCipher()

{

int i,j,round=0;

for(i=0;i<4;i++)
{
    for(j=0;j<4;j++)
    {
        state[j][i] = in[i*4 + j];
    }
}

AddRoundKey(Nr);
for(round=Nr-1;round>0;round--){
        InvShiftRows();
    InvSubBytes();
   AddRoundKey(round);
    InvMixColumns();
 
}
InvShiftRows();
InvSubBytes();
AddRoundKey(0);

for(i=0;i<4;i++)
    {   
        for(j=0;j<4;j++)
        {
            out[i*4+j]=state[j][i];
        }
    }
}

void encrypt1(char a[],char pass[],char b[]){
    int l=strlen(a);
	int l1=strlen(pass);
    int i,p=0;
	int i1,p1=0;
    int m=l%16;
	int m1=l1%16;
    FILE *fp=fopen(b,"w");
    if(m!=0){
        for(i=0;i<(16-m);i++){
            a[l]=' ';
            l=l+1;
        }
    }
    a[l]='\0';

	if(m1!=0){
		for(i1=0;i1<(16-m);i1++){
			pass[l1]=' ';
			l1=l1+1;
		}
	}
	pass[l1]='\0';

	for(i=0;i<Nk*4;i++){
		in[i]=pass[i];
	}
	Cipher();
	for(i1=0;i1<16;i1++){
		fputc((int)out[i1],fp);
	}

    while(p<l){
        for(i=0;i<Nk*4;i++)
	    {
            in[i]=a[i+p];
	    }
        Cipher();
        for(i=0;i<16;i++){
           // printf("%02x ",out[i]);
            fputc((int)out[i],fp);
        }

        p=p+16;
    }
    fclose(fp);
}

void decrypt1(char b[]){
    FILE *fp=fopen(b,"r");
    int c[1000],a;
    char q[16];
    int l=0,i=0,j=0;
    while((a=fgetc(fp))!=EOF){
        c[l]=(int)a; 
        l++;
    }
    c[l]='\0';
    fclose(fp);
	fp=fopen(b,"w");
    i=0;
    while(i<l){
        for(j=0;j<16;j++){
            in[j]=(char)c[i];
            i++;
        }
        InvCipher();
        for(j=0;j<16;j++){
			putc((char)out[j],fp);
        }
    }
	fclose(fp);
}

int abc(){

    Nr=128;
	Nk = Nr / 32;
	Nr = Nk + 6;
    int i=0;
	unsigned char temp[32] = {0x00  ,0x01  ,0x02  ,0x03  ,0x04  ,0x05  ,0x06  ,0x07  ,0x08  ,0x09  ,0x0a  ,0x0b  ,0x0c  ,0x0d  ,0x0e  ,0x0f};
	for(i=0;i<Nk*4;i++)
	{
		Key[i]=temp[i];
	}
    KeyExpansion();
	return 0;
}

void copy(FILE *source,FILE *target){           
    //Function that can copy a file source at a new location
    char ch;
    if( source == NULL )    //check if source file is not available
    {
      printf("file 1 not available");
    }
    while( ( ch = fgetc(source) ) != EOF )      //transfer character by character to new location     
      fputc(ch, target);    
    fclose(source);
    fclose(target);
}

int move(char source[],char target[]){          
    //Function that move the file using the copy function
  FILE *s,*t;
  s = fopen(source, "r");
  t = fopen(target, "w");
  copy(s,t);                //firstly copy to a new location
  remove(source);           //remove the original file
  return 1;
}


int ignoreCase(char a[],char b[]){              
    //Function used in searching by name to ignore the cases
    int c = 0;
 
   while (a[c] != '\0') {           //checking if it is not finished
      if (a[c] >= 'A' && a[c] <= 'Z') {     //capital alphabet is converted to smaller else ignore the alphabet
         a[c] = a[c] + 32;
      }
      c++;
   }
   c=0;
   while (b[c] != '\0') {       //doing the same with other character array that is making capital alphabet small
      if (b[c] >= 'A' && b[c] <= 'Z') {
         b[c] = b[c] + 32;
      }
      c++;
   }
    if(strcmp(a,b) == 0)        //checking for both strings
        return 1;
    else
        return 0;
    return 0;
}

int isSubstring(char a[],char b[]){     
    // a file name b substring to be searched
    int i,j,c=0,flag;
    int al=strlen(a);       //calculating length
    int bl=strlen(b);
    while (a[c] != '\0') {
      if (a[c] >= 'A' && a[c] <= 'Z') {     //case changing
         a[c] = a[c] + 32;
      }
      c++;
   }
   c=0;
   while (b[c] != '\0') {
      if (b[c] >= 'A' && b[c] <= 'Z') {
         b[c] = b[c] + 32;
      }
      c++;
   }
    for (i = 0; i <= al - bl; i++)      //checking for substrings
    {
        for (j = i; j < i + bl; j++)
        {
            flag = 1;
            if (a[j] != b[j - i])
            {
                flag = 0;
                break;
            }
        }
        if (flag == 1)
            break;
    }
    if (flag == 1)
        return 1;
    else
       return 0;
    return 0;
}


int extension(char filename[],char a[5][100]){      
    //checking if the extension for the file is one of the extension present in char array a
    int i=0;
    char * point = filename;
    if((point = strrchr(filename,'.')) != NULL ) {  
        for(i=0;i<10;i++){
            if(strcmp(point,a[i]) == 0) {
                return 1;
            }   
        }
    }
    return 0;
}

void check(char *dir,char ext[5][100])              
//Fucntion to traverse whole directory of system and check for extension
{
    char cwd[1024];
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;

    if((dp = opendir(dir)) == NULL) {
        return;
    }

    chdir(dir);             
    getcwd(cwd, sizeof(cwd));
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
           // printf("%*s%s/\n",depth,"",entry->d_name);

            /* Recurse at a new indent level */
            check(entry->d_name,ext);
        }
        else {
                if(extension(entry->d_name,ext)==1){          // file to be searched
                printf("%s\n",entry->d_name);
                printf("LOCATION: \t%s\n", cwd); 
                printf("\n");      
            }
        }
           // printf("%*s%s\n",depth,"",entry->d_name);
           // printf("Current working dir: %s\n", cwd);
    }
    chdir("..");
    closedir(dp);
}

void printdir(char *dir, char a[])          
//Fucntion to check the system file for a name
{
    char cwd[1024];
    DIR *dp;
    struct dirent *entry;
    struct stat statbuf;
    if((dp = opendir(dir)) == NULL) {
       
        return;
    }
    chdir(dir);             
    getcwd(cwd, sizeof(cwd));
    while((entry = readdir(dp)) != NULL) {
        lstat(entry->d_name,&statbuf);
        if(S_ISDIR(statbuf.st_mode)) {
            /* Found a directory, but ignore . and .. */
            if(strcmp(".",entry->d_name) == 0 ||
                strcmp("..",entry->d_name) == 0)
                continue;
           // printf("%*s%s/\n",depth,"",entry->d_name);

            /* Recurse at a new indent level */
            printdir(entry->d_name,a);
        }
        else {
                if(ignoreCase(entry->d_name,a) == 1 || isSubstring(entry->d_name,a) == 1){          // file to be searched
                printf("%s\n",entry->d_name);
                printf("LOCATION: \t%s\n", cwd); 
                printf("\n");      
            }
        }
           // printf("%*s%s\n",depth,"",entry->d_name);
           // printf("Current working dir: %s\n", cwd);
    }
    chdir("..");
    closedir(dp);
}

int FileEncounter(DIR *dr){             
    //Function to display the features provided for a file
    int a;
    printf("1. Delete the file");
    printf("2. Move the file");
    printf("3. Copy the file");
    printf("4. Encode the file");
    printf("5. Decode the file");
    scanf("%d",&a);
    return a;
}

int open1(DIR *dr){                 
    //Function to put the file names in a directory in text file for further reading
    struct dirent *de;
    FILE *fp;
    int a;
    fp=fopen("/Users/kushagrajindal/kj.txt","w");
    if ( fp == NULL )
    {   
        puts ( "Cannot open file" ) ;
        return 0;
    }
    if (dr == NULL)                 //if it is not a directory it is curtainly a file
    {
        a=FileEncounter(dr);
        return a;
    }
    while ((de = readdir(dr)) != NULL){     
           fputs(de->d_name,fp);            //copy name of file to the text file seperated by semicolon
           fputs(";",fp);
    }
    closedir(dr); 
    fclose(fp); 
    return 0;  
}


int read1(){            
    //read the files name written in the text file
    FILE *fp;
    int c=1,i=0;        //c is a counter variable for counting files in that directory
    char ch;
    char a[100];
    fp=fopen("/Users/kushagrajindal/kj.txt","r");
    if ( fp == NULL )
    {   
        puts ( "Cannot open file" ) ;
        return 0;
    }
    while(1){
        ch=fgetc(fp);
        if(ch==EOF){
            break;
        }
       if(ch==';'){         //if a semicolon is encountered it means that is a new file name
            printf("%d->\t",c);
            for(int j=0;j<i;j++){       //printing the file name
                printf("%c",a[j]);
            }

            printf("\n");
            i=0;
            c++;
        }
        else{
            a[i]=ch;
            i++;
        }
    }
    fclose(fp);
    return c;
}

char * readAtNumber(int n,char loc[]){          
    //read the file name at that position say n and then adding it to loc array thus creating the new path
    int j=0;
    FILE *fp;
    int p=0;
    int i=0;
    int a[1000];
    char ch;
    int l=strlen(loc);
    fp=fopen("/Users/kushagrajindal/kj.txt","r");
    if ( fp == NULL )
    {   
        puts ( "Cannot open file" ) ;
        return loc;
    }
    while(1){
       ch=fgetc(fp);
        if(ch==EOF){
            break;
        }
        if(ch==';'){
            if(p+1==n){
                 loc[l]='/';
   
                for(int j=0;j<i;j++){
                    loc[l+j+1]=a[j];

                 }
              
                return loc;
            }
            else{
                i=0;
                p++;
            }
        } 
        else{
            a[i]=ch;
            i++;
        }
    }
    fclose(fp);
    return loc;
}

int backbutton(char loc[1000]){
    char abc[1000];
    int l=strlen(loc),i,j;
    for(i=l-1;i>=0;i--){
        if(loc[i]=='/'){
            break;
        }
    }
    for(j=0;j<i;j++){
        abc[j]=loc[j];
    }
   return strlen(abc);
}

int main(){
    char location[1000]={'/'};
    int i=0,j=0,k,a,pos,ch,ppos;
    FILE *s,*t,*fi;
    DIR *dr;
    char *p,d;
    int c;
    char fname[100],des[100],pass1[100];
    char data[1000];
    dr=opendir(location);
    open1(dr);
    c=read1();
    printf("%d",c);
    while(i!=-1){
       printf("Enter a number of corresponding directory");
       scanf("%d",&i);
       if(i==c+7){
            printf("Thank You \n");
            return 0;
       }
       else if(i==c+6){     //back button
            k=backbutton(location);
            location[k]='\0';
            dr=opendir(location);
            open1(dr);
            c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
       }
       else if(i>=1 && i<c){
            p=readAtNumber(i,location);
            dr=opendir(p);
            int a=open1(dr);  
            if(a!=0){
                printf("%d",a);
                if(a==1){  //Delete a file
                    remove(location);
                    printf("Deleted");
                }
                else if(a==2){  //Move a file
                    printf("Enter the new location where uh want to move the file");
                    scanf("%s",des);
                    move(location,des);
                }
                else if(a==3){  //Copy a file
                    printf("Enter the new location where uh want to copy the file");
                    scanf("%s",des);
                    s = fopen(location, "r");
                    t = fopen(des, "w");
                    copy(s,t);
                }
                else if(a==4){  //Encode a file
                    fi=fopen(location,"r");
                    while((d=fgetc(fi))!=EOF){
                        data[pos]=d;
                        pos++;
                    }
                    data[pos]='\0';
                    abc();
                    printf("Enter the choice of password 1 for autogenerated and 2 for user define");
                    scanf("%d",&ch);
                    if(ch==1){
                        generate();
                        encrypt1(data,pass,location);
                        printf("Your password is %s",pass);
                    }
                    else{
                        printf("enter the password");
                        scanf("%s",pass);
                        encrypt1(data,pass,location);
                    }
                }
                else if(a==5){  //Decode a file
                    abc();
                    fi=fopen(location,"r");
                    while((d=fgetc(fi))!=EOF){
                        data[pos]=d;
                        pos++;
                    }
                    data[pos]='\0';
                    fclose(fi);
                    decrypt1(location);
                    fi=fopen(location,"r");
                    printf("enter the password");
                    scanf("%s",pass);
                    for(ppos=0;ppos<16;ppos++){
                        d=fgetc(fi);
                        if(d==' '){
                            break;
                        }
                        else{
                            pass1[ppos]=d;
                        }
                    }
                    pass1[ppos]='\0';
                    printf("%s",pass1);

                    if(strcasecmp(pass1,pass)==0){
                        
                    }
                    else{
                        printf("incorrect password");
                    }
                }
                else{           //Default
                    printf("Wrong choice entered");
                }
            }
            else{
               // dr=opendir(location);
               // open1(dr);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
       }
       else{
            if(i==c+4){             //searching with name or substring
                printf("Enter the name of file to be searched");
                scanf("%s",fname);
                printdir(location,fname);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }

            else if(i==c+5){            //searching with extension
                printf("Enter the name of extension to be searched");
                scanf("%s",fname);
                printdir(location,fname);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
        
            else if(i==c){          //searching for images
                char a[5][100]={".png",".jpg",".tif"};
                check(location,a);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
            else if(i==c+1){        //searching for videoes
                char a[5][100]={".avi",".mp4",".3gp"};
                check(location,a);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
            else if(i==c+2){        //searching for Audio
                char a[5][100]={".mp3"};
                check(location,a);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
            else if(i==c+3){        //searching for documents
                char a[5][100]={".docx",".pdf",".ppt",".txt",".pages"};
                check(location,a);
                c=read1();
                printf("%d->\tImages\n",c);
                printf("%d->\tVideos\n",c+1);
                printf("%d->\tAudio\n",c+2);
                printf("%d->\tDocuments\n",c+3);
                printf("%d->\tSearch With Name\n",c+4);
                printf("%d->\tSearch With Extension\n",c+5);
                printf("%d->\tback\n",c+6);
                printf("%d->\tQUIT\n",c+7);
                printf("%d",c);
            }
            else{
                printf("wrong choice");
            }

       }
      
    }
    return 0;
}
