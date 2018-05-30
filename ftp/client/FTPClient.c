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
 
#include <dirent.h>

long int p,q,n,pub_n,pub_e,t,flag,e[100],d[100],temp[100],j,m[100],i,len;
int en[100];
char en_msg[512];
char de_msg[100];
char send_msg[512];
int _index;
int private_d;
long int buffsize;

long int public_e;
long int public_n;

int _j;

void encrypt(long int pKey, long int nKey);
void generateKey();
int prime(long int);
void ce();
long int cd(long int);
void decrypt();

int main()
{
    FILE *fp;
    int csd,n,ser,s,cli,cport,newsd;
    char name[100],rcvmsg[100],rcvg[512],fname[100],fileread[100];
    struct sockaddr_in servaddr;
    char buffer[1025];

    printf("Masukkan port\t: ");
    scanf("%d",&cport);
    csd=socket(AF_INET,SOCK_STREAM,0);
    if(csd<0)
    {
        printf("Error....\n");
        exit(0);
    }
    bool rsa = true;

    memset(&servaddr, '0', sizeof(servaddr));

    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(cport);
    // Convert IPv4 and IPv6 addresses from text to binary form
	if(inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr)<=0) 
	{
		printf("\nInvalid address/ Address not supported \n");
		return -1;
	}

    if(rsa)
        generateKey();

    int mode = 1;

    if(connect(csd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0){
        printf("Error in connection\n");
        exit(0);
    }
    else{
        _index = 0;
        if(rsa){
            s=recv(csd,rcvg,512,0);
            // rcvg[s]='\0';
            strcpy(buffer, rcvg);
            // printf("#pesan: %s\n", buffer);
            char * segmen;
            segmen = strtok (buffer,":");
            // printf("#segmen: %s\n", segmen);
            while (segmen != NULL)
            {
                if (_index == 0)
                {
                    public_e = atoi(segmen);
                    printf("server e: %ld\n", public_e );
                    _index++;
                }
                else if (_index == 1)
                {
                    public_n = atoi(segmen);
                    printf("server n: %ld\n", public_n );
                    _index = 0;
                }
                segmen = strtok (NULL,":");
            }
        }

        printf("connected\n");
    }

    DIR *d;
    struct dirent *dir;
    d = opendir("/home/rangga/Documents/FTP_RSA-master/ftp/server");
    if (d) {
        printf("\nFile Di Server\n");
        while ((dir = readdir(d)) != NULL) {
            printf("%s\n", dir->d_name);
        }
        closedir(d);
    }

    system("clear");
    printf("1. Download\n");
    printf("2. Upload\n");
    printf("Option : ");
    scanf("%d", &mode);

    if(mode==1){
        send(csd,"download",8,0);
        usleep(1000);

        printf("Nama file yang akan didownload\t: ");
        scanf("%s",name);
        printf("Simpan file dengan nama\t\t: ");
        scanf("%s",fname);
        fp=fopen(fname,"w");

        // printf("buffer: %s\n", buffer);
        if(rsa){
            char sendbuffer[100];
            sprintf(sendbuffer, "%s:%ld:%ld", name, pub_e, pub_n);
            printf("..>%s\n", sendbuffer);
            send(csd,sendbuffer,sizeof(sendbuffer),0);
        }
        else{
            send(csd,name,sizeof(name),0);
        }
        usleep(500);

        while(1)
        {
            s=recv(csd,rcvg,512,0);
            rcvg[s]='\0';
            printf("----> pesan: %s\n", rcvg);
            if(strcmp(rcvg,"error")==0){
                printf("File yang anda download tidak ada\n");
                send(csd,"bye",3,0);

                fclose(fp);
                close(csd);
                break;
            }
                
            if(strcmp(rcvg,"completed")==0)
            {
                printf("\nBerhasil mendownload file!\n");
                
                send(csd,"bye",3,0);

                fclose(fp);
                close(csd);
                break;
            }
            
            if(strcmp(rcvg,"Welcome")==0)
            {
                printf("Welcome message recieved!\n");
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

                        fprintf(fp,"%s",de_msg);
                    }
                    else{
                        printf("langsung | size: %d\n", s);;
                        printf("Pesan: %s\n", rcvg);
                        fprintf(fp,"%s",rcvg);
                    }
                }
            }
        }
    }
    else{
        send(csd,"upload",6,0);
        usleep(1000);

        printf("Nama file yang akan diupload\t: ");
        scanf("%s",name);
        fp=fopen(name,"r");
        if(fp==NULL)
        {
            send(csd,"error",5,0);
            close(csd);
        }
        else
        {
            send(csd,name,strlen(name),0);
            usleep(2000);

            while(fgets(fileread,sizeof(fileread),fp))
            {
                printf("Pesannya : %s\n", fileread);
                len = strlen(fileread);
                
                if(rsa){
                    int ind=0;
                    for (ind=0;fileread[ind]!=NULL;ind++){
                        m[ind]= fileread[ind];
                        // printf("%c %ld, ", fileread[ind], m[ind]);
                    }
                    encrypt(public_e, public_n);
                    
                    // sprintf(send_msg, "%s|%s", name, en_msg);
                    printf("dikirim : %s, buffsize: %d\n", en_msg, buffsize);
                    // buffsize += strlen(name)+1;
                    if(send(csd, en_msg, buffsize,0)<0)
                    {
                        printf("Cant send\n");
                    }
                }
                else{
                    int _len = strlen(fileread);
                    printf("Pesan : %s, panjang: %d\n", fileread, _len);
                    _len += strlen(name)+1;
                    sprintf(send_msg, "%s|%s", name, fileread);
                    if(send(csd, send_msg, _len,0)<0)
                    {
                        printf("Cant send\n");
                    }

                    usleep(500);
                }
                
                usleep(500);
            }

            printf("Sent\n");
            send(csd,"bye",3,0);

            fclose(fp);
            close(csd);
        }
    }
    
        
    
}

void encrypt(long int pKey, long int nKey) {
    long int pt,ct,key=pKey,k;
    int i = 0;
    n = nKey;
	
    printf("len: %ld, key: %ld\n", len, key);
    while(i!=len) {
        pt=m[i];
        k=1;
 
		for (j=0;j<key;j++) {
            k=k*pt;
            k=k%n;
		}

		en[i]=k;
		// printf("(%ld:%c:%ld), ", i, (char)m[i], en[i]);
        i++;
	}
    // printf("\n");
 
	en[i]=-1;

    int _j=0;
    for (i=0;en[i]!=-1;i++){
	    long int aq = en[i];
        // char b[500];
        
        // if(aq >=0 ){
            // printf("aq : %ld\n", aq);
            int ex=0;
            for(ex=0; ex<4; ex++){
                int tmp = (int) 127 & aq;
                aq = aq >> 7;

                en_msg[_j] = tmp;

                // printf("\t\tb%d : %d, tmp: %d\n", _j, en_msg[_j], tmp);
                // printf("\t\ta now : %ld\n", aq);
                _j++;
            }
        // }
    }

    buffsize = _j;
    printf("Buffer size: %ld\n", buffsize);
    // int nj;
    // printf("======================\n");
    // for(nj=_j-1; nj>=0; nj--){
    //     printf("%d.", en_msg[nj]);
    //     // printf("a now : %ld\n", a);
    // }
    // printf("\n");
    // sprintf(en_msg, "%s", _j, en_msg);
    // printf("message: %s\n", en_msg);
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

    pub_e = e[_index-1];
    private_d = d[_index-1];
    printf("Public key: (%ld, %ld)\n", pub_e, pub_n);
    printf("Private key: (%d, %ld)\n", private_d, pub_n);
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
    printf("\nkey: %ld, n: %ld\n", key, pub_n);
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