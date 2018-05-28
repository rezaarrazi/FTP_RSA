#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>

#include<stdio.h>
// #include<conio.h>    //untuk windows
#include <curses.h>     //untuk unix
#include<stdlib.h>
#include<math.h>
#include<string.h>
 
long int p,q,pub_n,t,flag,e[100],d[100],temp[100],j,m[100],i;
int en[100];
char de_msg[100];
int _index;
int private_d;

int _j;

void generateKey();
int prime(long int);
void ce();
long int cd(long int); 
void encrypt();
void decrypt();

int main()
{
    FILE *fp;
    int csd,n,ser,s,cli,cport,newsd;
    char name[100],rcvmsg[100],rcvg[512],fname[100];
    struct sockaddr_in servaddr;
    printf("Masukkan port\t: ");
    scanf("%d",&cport);
    csd=socket(AF_INET,SOCK_STREAM,0);
    if(csd<0)
    {
        printf("Error....\n");
        exit(0);
    }

    memset(&servaddr, '0', sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(cport);
    // Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "192.168.200.68", &servaddr.sin_addr)<=0) 
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}
    
    if(connect(csd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("Error in connection\n");
        exit(0);
    }
    else
        printf("connected\n");

    bool rsa = false;

    if(rsa)
        generateKey();

    printf("Nama file yang akan didownload\t: ");
    scanf("%s",name);
    printf("Simpan file dengan nama\t\t: ");
    scanf("%s",fname);
    fp=fopen(fname,"w");

    // printf("buffer: %s\n", buffer);
    if(rsa){
        char buffer[100];
        sprintf(buffer, "%s:%ld:%ld", name, e[_index-1], pub_n);
        send(csd,buffer,sizeof(buffer),0);
    }
    else{
        send(csd,name,sizeof(name),0);
    }
        
    while(1)
    {
        s=recv(csd,rcvg,512,0);
        rcvg[s]='\0';
        if(strcmp(rcvg,"error")==0)
            printf("File yang anda download tidak ada\n");
        if(strcmp(rcvg,"completed")==0)
        {
            printf("\nBerhasil mendownload file!\n");
            fclose(fp);
            close(csd);
            break;
        }
        else{
            if(s>0){
                if(rsa){
                    printf("size: %d\n", s);
                    int ex;
                    // for(ex=s-1; ex>=0; ex--){
                    //     printf("%d.", rcvg[ex]);
                    // }
                    long int nChar = s/4;

                    for(ex=0; ex<nChar; ex++){
                        long int a = 0;
                        int jInd;
                        // printf("\n======================\n");
                        // printf("->a now : %ld\n", a);
                        for( jInd = 3; jInd>=0; jInd--){
                            a = a << 7;
                            a = a | rcvg[(4*ex) + jInd];
                            // printf("data[%d]:%d| ", (4*ex) + jInd, rcvg[(4*ex) + jInd]);
                        }
                        // printf("\na now : %ld\n", a);
                        en[ex] = a;
                    }
                    en[nChar] = -1;
                    decrypt();
                }
                else{
                    fputs(rcvg,stdout);
                }
            }
        }
        if(rsa) fprintf(fp,"%s",de_msg);
    }
}

void generateKey(){
    printf("Masukkan bilangan prima pertama: ");
    scanf("%ld",&p);

	flag=prime(p);
    if(flag==0) {
    	printf("INPUT BUKAN BILANGAN PRIMA!\n");
 
		exit(1);
	}

	printf("Masukkan bilangan prima kedua: ");
	scanf("%ld",&q);

	flag=prime(q);
	if(flag==0||p==q) {
        printf("INPUT BUKAN BILANGAN PRIMA!\n");
 
		exit(1);
	}
 
	pub_n=p*q;
    t=(p-1)*(q-1);
    // printf("n: %ld", pub_n);

	ce();

	printf("POSSIBLE VALUES OF e AND d ARE\n");
	for (i=0;i<j-1;i++)
        printf("%ld) %ld\t%ld\n",i+1,e[i],d[i]);

    printf("Pilih key: ");
    scanf("%d", &_index);
    if(_index < 0 || _index >= (int)j){
        printf("Out of index\n");
        return;
    }

    private_d = d[_index-1];
    printf("Public key: (%ld, %ld)\n", e[_index-1], pub_n);
    printf("Private key: (%ld, %ld)\n", d[_index-1], pub_n);

	// encrypt();
	// decrypt();
}

int prime(long int pr) {
    int i;
 
	j=sqrt(pr);
    for (i=2;i<=j;i++) {
    	if(pr%i==0)
            return 0;
	}
 
	return 1;
}
 
void ce() {
    int k = 0;
	for (i=2;i<t;i++) {
        if(t%i==0)
            continue;
 
		flag=prime(i);
        if(flag==1&&i!=p&&i!=q) {
            e[k]=i;
            flag=cd(e[k]);
 
			if(flag>0) {
                d[k]=flag;
                k++;
            }
 
			if(k==99)
                break;
        }
	}
}
 
long int cd(long int x) {
	long int k=1;
	while(1) {
        k=k+t;
        if(k%x==0)
            return(k/x);
	}
}
 
void decrypt() {
	long int pt,ct,key=private_d,k;
	i=0;
    // printf("\nkey: %ld, n: %ld\n", key, n);
    // printf("\ndecrypt\n");
	while(en[i]!=-1) {
        ct = en[i];
        
        k=1;
 
		for (j=0;j<key;j++) {
            k=k*ct;
            k=k%pub_n;
		}
 
		// printf("%ld", pt);
        m[i]=k;
        de_msg[i] = k;
		i++;
	}
    // printf("\n");
	
    m[i]=-1;
    printf("\nTHE DECRYPTED MESSAGE IS\n");
    printf("%s\n",de_msg);
	// for (i=0;m[i]!=-1;i++){
    //     printf("%c",(char) m[i]);
    // }
    printf("\n");
}