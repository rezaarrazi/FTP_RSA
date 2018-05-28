#include<stdio.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<string.h>
#include<stdlib.h>

#include<stdio.h>
// #include<conio.h>    //untuk windows
#include <curses.h>     //untuk unix
#include<stdlib.h>
#include<math.h>
#include<string.h>
 
long int p,q,n,t,flag,e[100],d[100],temp[100],j,m[100],en[100],i,len;
char en_msg[512];
int _index;
long int public_e;
long int public_n;
long int buffsize;

void encrypt();

int main()
{
    FILE *fp;
    int sd,newsd,ser,n,a,cli,pid,bd,port,clilen;
    char name[100],fileread[100],fname[100],ch,file[100],rcv[100];
    struct sockaddr_in servaddr,cliaddr;
    bool rsa = false;
    int opt = 1;

    printf("Masukkan port\t: ");
    scanf("%d",&port);
    sd=socket(AF_INET,SOCK_STREAM,0);
    if(sd<0)
        printf("Gagal membuat socket\n");
    else
        printf("Berhasil membuat socket pada %d\n", port);
    
    // Forcefully attaching socket to the port 8080
    if (setsockopt(sd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    cliaddr.sin_family=AF_INET;
    cliaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    cliaddr.sin_port=htons(port);
    a=sizeof(cliaddr);
    bd=bind(sd,(struct sockaddr *)&cliaddr,a);

    if(bd<0)
        printf("Cant bind\n");
    else
        printf("Binded\n");
    
    listen(sd,5);
    clilen=sizeof(cliaddr);
    newsd=accept(sd,(struct sockaddr *)&cliaddr,&clilen);
    if(newsd<0)
    {
        printf("Cant accept\n");
    }
    else
        printf("Accepted\n");

    n=recv(newsd,rcv,150,0);
    rcv[n]='\0';
    
    if(rsa){
        char filename[100];

        char * segmen;
        segmen = strtok (rcv,":");
        while (segmen != NULL)
        {
            if (_index == 0) 
            {
                printf("filename: %s\n", segmen );
                strcpy(filename, segmen);
                _index++;
            }
            else if (_index == 1)
            {
                public_e = atoi(segmen);
                printf("e: %ld\n", public_e );
                _index++;
            }
            else if (_index == 2)
            {
                public_n = atoi(segmen);
                printf("n: %ld\n", public_n );
                _index++;
            }
            segmen = strtok (NULL,":");
        }

        fp=fopen(filename,"r");
    }
    else{
        fp=fopen(rcv,"r");
    }

    if(fp==NULL)
    {
        send(newsd,"error",5,0);
        close(newsd);
    }
    else
    {
        while(fgets(fileread,sizeof(fileread),fp))
        {
            printf("Pesan : %s\n", fileread);
            len = strlen(fileread);
            
            if(rsa){
                int ind=0;
                for (ind=0;fileread[ind]!=NULL;ind++){
                    m[ind]= fileread[ind];
                    // printf("%c %ld, ", fileread[ind], m[ind]);
                }
                // for (ind=0;m[ind]!=NULL;ind++){
                //     printf("%ld, ", m[ind]);
                // }
                // printf("\n");
                encrypt();

                if(send(newsd,en_msg,buffsize,0)<0)
                {
                    printf("Cant send\n");
                }
            }
            else{
                if(send(newsd,fileread,len,0)<0)
                {
                    printf("Cant send\n");
                }
            }
            
            sleep(1);
        }
        // if(!fgets(fileread,sizeof(fileread),fp))
        // {
            send(newsd,"completed",999999999,0);
        // }
        return(0);
    }
}

void encrypt() {
    long int pt,ct,key=public_e,k;
    i = 0;
    n = public_n;
	
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