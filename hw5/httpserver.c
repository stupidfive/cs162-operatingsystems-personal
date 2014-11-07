/* A simple HTTP server with port number is passed as an argument */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h> 
 
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

#define MAXPATH 1024
#define MAXBUF 10240
#define MAXREQ  8192
#define MAXQUEUE 5

void error(char *msg)
{
    perror(msg);
    exit(1);
}

const char dir_listing_head[] = 
"<html>"
"<head>"
"<title>index of %s </title>"
"</head>"
"<body>"
"<h1> Index of %s</h1>"
"<table>"
"<tr> <th style='width:300px'> Name </th> <th style='width:300px'> Type </tr>"; // %s refers to the directory name

const char dir_file_entry[] = 
"<tr> <td>%s</td> <td>Regular File</td> </tr>"; // %s refers to a file name within the directory
const char dir_dir_entry[] = 
"<tr> <td>%s</td> <td>Directory</td> </tr>"; // %s refers to a sub-directory name within the directory
const char dir_listing_end[] = 
"</table> </body> </html>\r\n\r\n";

const char htmlheader[]="HTTP/1.0 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "\r\n";

const char textheader[]="HTTP/1.0 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "\r\n";

const char header404[]="HTTP/1.0 404 Not Found\r\n"
  "Content-Type: text/html\r\n"
  "\r\n";

const char header400[]="HTTP/1.0 400 Bad Request\r\n"
  "Content-Type: text/html\r\n"
  "\r\n";

struct stat st;

int process_http_request(int httpsockfd)
{
  char reqbuf[MAXREQ];
  int n=0;
  /* Note this is same as the HW2 skeleton.
   * Replace this with your HW2 implementation
   * */

  memset(reqbuf,0, MAXREQ);
  n = read(httpsockfd,reqbuf,MAXREQ-1);
  char * p;
  p = strtok(reqbuf, " ");
  char *path;
  char *c = (char *) malloc(sizeof(char));;
  FILE *file;
  int comp = 1;
  comp = strcmp(p, "GET");
  if (!comp) { //is this a GET request
    p = strtok(NULL, " ");
    path = (char *) malloc(strlen(p) + 3);
    sprintf(path, "%s%s", "www", p);
    file = fopen(path, "r");
    if (file) { //file exists
			fstat(fileno(file), &st);
			if (S_ISREG(st.st_mode)) {
				write(httpsockfd, htmlheader, strlen(htmlheader));
			} else if (S_ISDIR(st.st_mode)) {
				char *index_path = (char *) malloc(strlen(path) + 11);
				sprintf(index_path, "%s%s", path, "/index.html"); //still need to check last symbol
				file = fopen(index_path, "r");
				if (!file) {
					//build directory
				}
				write(httpsockfd, htmlheader, strlen(htmlheader));
    } else { //file doesn't exist
      write(httpsockfd, header404, strlen(header404));
      file = fopen("404.html", "r");
    }
    free(path);
  } else { //not a GET request
    write(httpsockfd, header400, strlen(header400));
    file = fopen("400.html", "r");
  }

  //send file
  *c = fgetc(file);
  while (*c != EOF) {
    write(httpsockfd, c, 1);
    *c = fgetc(file);
  }
	fclose(file);

	return 0;
}

int sockfd, newsockfd;    /* make static so signal handler can close */

void signal_callback_handler(int signum)
{
  printf("Caught signal %d\n",signum);
  printf("Close socket %d\n", sockfd);
  if (close(sockfd) < 0) perror("failed to close sockfd\n");
  exit(signum);
}

int server(int portno)
{
  struct sockaddr_in serv_addr; /* internet style socket address object */
  struct sockaddr_in cli_addr;
  uint clilen = sizeof(cli_addr);

  pid_t cpid;
  int socketOption = 1;

  signal(SIGINT, signal_callback_handler);

  /* Create Socket to receive requests*/
  sockfd = socket(PF_INET, SOCK_STREAM, 0);
  if (sockfd < 0) error("ERROR opening socket");
  printf("Got socket: %d\n", sockfd);

  if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &socketOption, sizeof(socketOption)))
    error("ERROR setting reuseadd option");

  /* Bind socket to port */
  memset((char *) &serv_addr,0,sizeof(serv_addr));
  serv_addr.sin_family      = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port        = htons(portno);
  if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    close(sockfd);
    error("ERROR on binding");
  }

  while (1) {
    listen(sockfd,MAXQUEUE);    /* Listen for incoming connections */

    /* Accept incoming connection, obtaining a new socket for it */
    if ((newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen)) < 0)
      error("ERROR on accept");
    printf("new socket: %d\n", newsockfd);

    cpid = fork();         /* create new process for connection */
    if (cpid > 0) {     /* parent process */
      close(newsockfd);     /* parent drops refernce on connection socket */
      printf("Ready for next\n");
    } else if (cpid == 0) {   /* child process */
      close(sockfd);      /* clild drops reference on listening socket */
      process_http_request(newsockfd);
      close(newsockfd); /* child drops refernce on connection socket */
      exit(EXIT_SUCCESS); /* exit child normally */
    } else {      /* fork failed */
      error("Fork of child process failed\n");
    }
  }
  printf("Server exiting\n");
  close(sockfd);
  return 0; 
}


int main(int argc, char *argv[])
{
  int portno;
  if (argc < 2) {
    fprintf(stderr,"usage %s portno\n", argv[0]);
    exit(1);
  }
  portno = atoi(argv[1]);
  printf("Opening server on port %d\n",portno);
  return server(portno);
}
