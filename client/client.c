#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#define currentDir  getcwd


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




int main()
{

    //create a socket
	int sock;
	sock = socket(AF_INET,SOCK_STREAM,0);

    //setup the server information
	int option=1;
	setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&option,sizeof(option));
	struct sockaddr_in server_address;
	server_address.sin_family =AF_INET;
	server_address.sin_port =htons(9002);
	server_address.sin_addr.s_addr=INADDR_ANY;

    
    //connect to the server
	int connection_status= connect(sock, (struct sockaddr*) &server_address, sizeof(server_address));

	if(connection_status==-1){

		printf("error");
		close(sock);
		exit(-1);
	}



	//receive data from the server 
	char server_response[4096];
	recv(sock, &server_response,sizeof(server_response),0);
	

	//prit out to the server's response
    printf("You can select following files that listed below\n");
	printf(" Files on the server: %s\n",server_response);


	char filename[100];
	char fileFormat;


	//Send the file name that request from the server
	printf("\nEnter the file name to get it :");
	scanf("%s",filename);
	send(sock, filename, sizeof(filename),0);


	
    //to find the type of request file 
    fileFormat=getFileFormat(filename, sizeof(filename));
    
    
    //txt file

	if(fileFormat=='t')
	{

        recv(sock, &server_response,sizeof(server_response),0); 

		FILE *fp;
		char ch;

		//save the data in data.txt
		fp= fopen(filename,"w");
		fprintf(fp,"%s",server_response);

		fclose(fp);

		//the location of the file
		char buff[FILENAME_MAX];
		currentDir(buff,FILENAME_MAX);
		printf("\nThe file is saved in this directory : %s \n",buff);			
	}


    //pgm file

	else if(fileFormat=='p')
	{


		char sended[40000];
	    recv(sock, &sended,sizeof(sended),0);
		FILE *fp;
		int size,i,j;
		int data[200][200];
		char ch;
	
		fp= fopen(filename,"wb");
		
		fprintf(fp, "P2\n");

		fprintf(fp,"%d %d\n", 200,200);

		fprintf(fp, "255\n");
		int cnt=0;
		for(i=0;i<200;i++) 
		{
			for(j=0;j<200;j++) 
			{
				fprintf(fp, "%d ", atoi(&sended[j*(i+1)+i]));

			}
			fprintf(fp, "\n");
		}

		fclose(fp);
	}


    //html file

    else  if(fileFormat=='h')
    {
       
        recv(sock, &server_response,sizeof(server_response),0);
        

		//save the data 
		FILE *fp;
		char ch;
		fp= fopen(filename,"wb");
		fprintf(fp,"%s",server_response);
		fclose(fp);

		//the location of the txt file
		char buff[FILENAME_MAX];
		currentDir(buff,FILENAME_MAX);
		printf("\nThe file is saved in this directory : %s \n",buff);

    }



	else
	{
		printf("\nunknown format : %c",fileFormat);
	}


	close(sock);

	return 0;
}
