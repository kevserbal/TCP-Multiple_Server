#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <string.h>


//find the file format
char getFileFormat(char filename [], int filenamesize)
{

	int i =0;
	char format='n';
	
	for (i=0;i<filenamesize;i++)
	{
		if(filename[i]=='.')
			format=filename[i+1];
	}
	return format;
}



int main(){ 

	char server_message[256]= "data.txt,index.html,image.pgm";

    
    //create a socket
	int option=1;
	int server_socket;
	server_socket = socket(AF_INET, SOCK_STREAM, 0);


    //bind to a socket, communicate with local host
	setsockopt(server_socket ,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
	struct sockaddr_in server_address;
	server_address.sin_family =AF_INET;
	server_address.sin_port= htons(9002);
	server_address.sin_addr.s_addr=INADDR_ANY;

    bind(server_socket, (struct sockaddr*) &server_address, sizeof(server_address));


    //listen the connection queue, the maximum number of connection request in the queue is 5
	listen(server_socket,5);
	

    //server_socket is interest with other connection in the queue, the second one is client_socket which is interest with current request
    //Then server sends a message to the client which data file is requested
	int client_socket;
	client_socket =accept(server_socket, NULL,NULL);
    setsockopt(client_socket ,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
	send(client_socket, server_message, sizeof(server_message),0);


	//Get the requested filename
	char filename[100];
	recv(client_socket, &filename,sizeof(filename),0);

	char fileFormat=getFileFormat(filename, sizeof(filename));

	
    //pgm file

    if(fileFormat=='p') 
	{
		FILE *fp;
		char ch;

		fp= fopen(filename,"r");

		if(fp==NULL)
		{
			close(server_socket);
			close(client_socket);
			exit(-1);
		}
       
		int size = 200*200,i=0;
		char data[40000];

		while(i==size-1){
			
			char c =fgetc(fp);
			data[i]=c;
		}
		fclose(fp);

		//Send the file to the client
		send(client_socket, data,sizeof(data),0);

        close(client_socket);
	}



    //html file

	else if (fileFormat=='h') 
    {
        FILE *fhtml;
        fhtml=fopen(filename,"r");

        char response_data[4096];
        fscanf(fhtml,"%s",response_data);

        char http_header[4096] = "HTTP/1.1 200 OK\r\n\n" ;

        printf("the file name : %s \n",response_data);

        strcat(http_header,response_data);

        send(client_socket, http_header, sizeof(http_header),0);

        close(client_socket);
    }



    //txt file

	else if(fileFormat=='t') 
    {
        FILE *txt;
        txt=fopen(filename,"r");
        char response_data[1024];
		fscanf(txt,"%s",response_data);
		fclose(txt);

        printf("the file name : %s \n",filename);


        send(client_socket, response_data, sizeof(response_data),0);

		fclose(txt);

        close(client_socket);
    }



	else
	{
		printf("\n UNKOWN FILE FORMAT");

	}
	

	close(server_socket);

	return 0;
}
