//Example code: A simple server side code, which echos back the received message.
//Handle multiple socket connections with select and fd_set on Linux 
#include <stdio.h> 
#include <string.h> //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h> //close 
#include <arpa/inet.h> //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 

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
 
long int p,q,n,t,flag,e[100],d[100],temp[100],j,m[100],en[100],len,pub_n,pub_e;
char en_msg[512];
char in_buff[512];
int _index;
long int public_e[30];
long int public_n[30];
long int buffsize;

int mode[30];

char de_msg[100];
int private_d;
int _j;

	
#define TRUE 1 
#define FALSE 0 
#define PORT 2222 

void encrypt(long int pKey, long int nKey);
void generateKey();
int prime(long int);
void ce();
long int cd(long int); 
void decrypt();
	
int main(int argc , char *argv[]) 
{ 
	int opt = TRUE; 
	int master_socket , addrlen , new_socket , client_socket[30] , 
		max_clients = 30 , activity, i , valread , sd; 
	int max_sd; 
	
	FILE *fp;
	int newsd,ser,n,a,cli,pid,bd,port,clilen;
	char name[100],fileread[100],fname[100],ch,file[100],rcv[100];
	struct sockaddr_in servaddr,cliaddr;
	
	bool rsa = true;

	struct sockaddr_in address; 
		
	char buffer[1025]; //data buffer of 1K 
		
	//set of socket descriptors 
	fd_set readfds; 
		
	//a message 
	char *message = "Welcome"; 

	if(rsa)
        generateKey();
	
	//initialise all client_socket[] to 0 so not checked 
	for (i = 0; i < max_clients; i++) 
	{ 
		client_socket[i] = 0; 
	} 
		
	//create a master socket 
	if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	//set master socket to allow multiple connections , 
	//this is just a good habit, it will work without this 
	if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
		sizeof(opt)) < 0 ) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	
	//type of socket created 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
		
	//bind the socket to localhost port 8888 
	if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	printf("Listener on port %d \n", PORT); 
		
	//try to specify maximum of 3 pending connections for the master socket 
	if (listen(master_socket, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
		
	//accept the incoming connection 
	addrlen = sizeof(address); 
	puts("Waiting for connections ..."); 
		
	while(TRUE) 
	{ 
		//clear the socket set 
		FD_ZERO(&readfds); 
	
		//add master socket to set 
		FD_SET(master_socket, &readfds); 
		max_sd = master_socket; 
			
		//add child sockets to set 
		for ( i = 0 ; i < max_clients ; i++) 
		{ 
			//socket descriptor 
			sd = client_socket[i]; 
				
			//if valid socket descriptor then add to read list 
			if(sd > 0) 
				FD_SET( sd , &readfds); 
				
			//highest file descriptor number, need it for the select function 
			if(sd > max_sd) 
				max_sd = sd; 
		} 
	
		//wait for an activity on one of the sockets , timeout is NULL , 
		//so wait indefinitely 
		activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL); 
	
		if ((activity < 0) && (errno!=EINTR)) 
		{ 
			printf("select error"); 
		} 
			
		//If something happened on the master socket , 
		//then its an incoming connection 
		if (FD_ISSET(master_socket, &readfds)) 
		{ 
			if ((new_socket = accept(master_socket, 
					(struct sockaddr *)&address, (socklen_t*)&addrlen))<0) 
			{ 
				perror("accept"); 
				exit(EXIT_FAILURE); 
			} 
			
			//inform user of socket number - used in send and receive commands 
			printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
				(address.sin_port)); 
		
			if(rsa){
				char buffer[100];
				sprintf(buffer, "%ld:%ld", pub_e, pub_n);
				send(new_socket, buffer,sizeof(buffer),0);
			}
			// //send new connection greeting message 
			// if( send(new_socket, message, strlen(message), 0) != strlen(message) ) 
			// { 
			// 	perror("send"); 
			// }
			usleep(500);
				
			// puts("Welcome message sent successfully"); 
				
			//add new socket to array of sockets 
			for (i = 0; i < max_clients; i++) 
			{ 
				//if position is empty 
				if( client_socket[i] == 0 ) 
				{ 
					client_socket[i] = new_socket; 
					printf("Adding to list of sockets as %d\n" , i); 
						
					break; 
				} 
			} 
		} 
			
		for (i = 0; i < max_clients; i++) 
		{ 
			sd = client_socket[i]; 
				
			if (FD_ISSET( sd , &readfds)) 
			{ 
                valread = read( sd , buffer, 1024);
                buffer[valread] = '\0';
				// printf("buffer: %s, valread: %d\n", buffer, valread);
				
				if( strcmp("download", buffer) == 0) 
				{
					// printf("down\n");
					mode[i] = 1;
					break;
				}
				else if( strcmp("upload", buffer) == 0) 
				{
					// printf("up\n");
					mode[i] = 2;
					break;
				}

				if(mode[i] == 1){
					if ( strcmp("bye", buffer) == 0) 
					{ 
						//Somebody disconnected , get his details and print 
						getpeername(sd , (struct sockaddr*)&address , \
							(socklen_t*)&addrlen); 
						printf("Host disconnected , ip %s , port %d \n" , 
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
							
						//Close the socket and mark as 0 in list for reuse 
						close( sd ); 
						client_socket[i] = 0; 
						public_e[i] = 0;
						public_n[i] = 0;
					}
					else
					{ 
						//set the string terminating NULL byte on the end 
						//of the data read 
						
						if(rsa){
							char filename[100];
							_index = 0;
							strcpy(rcv, buffer);
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
									public_e[i] = atoi(segmen);
									printf("e: %ld\n", public_e[i] );
									_index++;
								}
								else if (_index == 2)
								{
									public_n[i] = atoi(segmen);
									printf("n: %ld\n", public_n[i] );
									_index = 0;
								}
								segmen = strtok (NULL,":");
							}

							fp=fopen(filename,"r");

							if(fp==NULL)
							{
								send(sd,"error",5,0);
								close(sd);
							}
							else
							{
								while(fgets(fileread,sizeof(fileread),fp))
								{
									printf("RSA Pesan : %s\n", fileread);
									len = strlen(fileread);
									
									if(rsa){
										int ind=0;
										for (ind=0;fileread[ind]!=NULL;ind++){
											m[ind]= fileread[ind];
											// printf("%c %ld, ", fileread[ind], m[ind]);
										}
										encrypt(public_e[i], public_n[i]);

										if(send(sd,en_msg,buffsize,0)<0)
										{
											printf("Cant send\n");
										}
									}
									
									usleep(500);
								}

								printf("Sent\n");
								if(send(sd,"completed",9,0) < 0) printf("not done\n");
								else printf("Done\n");
							}

							fclose(fp);
						}
						else{
							fp=fopen(buffer,"r");

							if(fp==NULL)
							{
								send(sd,"error",5,0);
								close(sd);
							}
							else
							{
								while(fgets(fileread,sizeof(fileread),fp))
								{
									len = strlen(fileread);
									printf("Pesan : %s, panjang: %ld\n", fileread, len);
									if(send(sd,fileread,len,0)<0)
									{
										printf("Cant send\n");
									}

									usleep(500);
								}

								printf("Sent\n");
								if(send(sd,"completed",9,0) < 0) printf("not done\n");
								else printf("Done\n");
							}

							fclose(fp);
						}
						
						//send(sd , buffer , strlen(buffer) , 0 ); 
					}
				}
				else if(mode[i] == 2){
					if ( strcmp("bye", buffer) == 0) 
					{ 
						//Somebody disconnected , get his details and print 
						getpeername(sd , (struct sockaddr*)&address , \
							(socklen_t*)&addrlen); 
						printf("Host disconnected , ip %s , port %d \n" , 
							inet_ntoa(address.sin_addr) , ntohs(address.sin_port)); 
							
						//Close the socket and mark as 0 in list for reuse 
						close( sd );
						fclose(fp);
						
						client_socket[i] = 0; 
						public_e[i] = 0;
						public_n[i] = 0;
					}
					else{
						if(buffer[valread-1] == 't' &&  buffer[valread-2] == 'x' && buffer[valread-3] == 't'){
							printf("filename: %s\n", buffer );
							char filename[100];
							strcpy(filename, buffer);

							fp=fopen(filename,"w");
						}
						else{
							if(rsa){
								printf("size: %d\n", valread);
								int ex;
								// for(ex=valread-1; ex>=0; ex--){
								//     printf("%d.", buffer[ex]);
								// }
								long int nChar = valread/4;

								for(ex=0; ex<nChar; ex++){
									long int a = 0;
									int jInd;
									// printf("\n======================\n");
									// printf("->a now : %ld\n", a);
									for( jInd = 3; jInd>=0; jInd--){
										a = a << 7;
										a = a | buffer[(4*ex) + jInd];
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
								fprintf(fp,"%s",buffer);
								_index=0;
							}
						}
					}
				}
			} 
		} 
	} 
		
	return 0; 
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
	int i = 0;

	printf("POSSIBLE VALUES OF e AND d ARE\n");
	for (i=0;i<j-1;i++)
        printf("%d) %ld\t%ld\n",i+1,e[i],d[i]);

    printf("Pilih key: ");
    scanf("%d", &_index);
    if(_index < 0 || _index >= (int)j){
        printf("Out of index\n");
        return;
    }

    private_d = d[_index-1];
	pub_e = e[_index-1];
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
	int i = 0;
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
	int i = 0;
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