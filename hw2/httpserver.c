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

typedef enum { false, true } bool;

const char htmlheader[]="HTTP/1.1 200 OK\r\n"
  "Content-Type: text/html\r\n"
  "\r\n";

const char textheader[]="HTTP/1.1 200 OK\r\n"
  "Content-Type: text/plain\r\n"
  "\r\n";

const char error400[]=
  "HTTP/1.1 400 Bad Request\r\n\r\n"
  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
  "<html>\r\n"
  "<head>\r\n"
  "   <title>400 Bad Request</title>\r\n"
  "</head>\r\n"
  "<body>\r\n"
  "   <h1>Bad Request</h1>\r\n"
  "   <p>The request was bad</p>\r\n"
  "</body>\r\n"
  "</html>\r\n";

const char error403[]=
  "HTTP/1.1 403 Unauthorized\r\n\r\n"
  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
  "<html>\r\n"
  "<head>\r\n"
  "   <title>403 Unauthorized </title>\r\n"
  "</head>\r\n"
  "<body>\r\n"
  "   <h1>Forbidden</h1>\r\n"
  "   <p>You don't have permission to access this directory on this server</p>\r\n"
  "</body>\r\n"
  "</html>\r\n";

const char error404[]=
  "HTTP/1.1 404 Not Found\r\n\r\n"
  "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">\r\n"
  "<html>\r\n"
  "<head>\r\n"
  "   <title>404 Not Found</title>\r\n"
  "</head>\r\n"
  "<body>\r\n"
  "   <h1>Not Found</h1>\r\n"
  "   <p>The requested URL /t.html was not found on this server.</p>\r\n"
  "</body>\r\n"
  "</html>\r\n";

typedef struct dirlisting { 
    char name[MAXPATH]; // filename
    bool isdir; // true if it is a directory , else it is a file
} dirlisting_t;

// Takes as parameters a directory name, list of files in the directory, and the number of files
// Returns a pointer to a formatted webpage
char* format_directory_listing( char *dirname, dirlisting_t *directory, int numfiles) {
    /* dirname : name of directory
     * directory : list of files in the directory
     * numfiles : number of files
     */

    char *webpage = malloc(MAXBUF * sizeof(char));
    webpage[0] = '\0';
    strcat(webpage, htmlheader);
    strcat(webpage, "<html>\r\n");
    strcat(webpage, "<head>\r\n");
    strcat(webpage, "<title>Index of ");
    strcat(webpage, dirname);
    strcat(webpage, " </title>\r\n");
    strcat(webpage, "</head>\r\n");
    strcat(webpage, "<body>\r\n");
    strcat(webpage, "<h1>\r\n");
    strcat(webpage, "Index of ");
    strcat(webpage, dirname);
    strcat(webpage, "</h1>\r\n");
    strcat(webpage, "<table> \r\n <tr> \r\n <th style='width:300px'> Name </th> <th style='width:300px'> Type \r\n </tr> \r\n");
    
    int i = 0;
    for(i=0; i<numfiles; i++) {
        strcat(webpage, "<tr> \r\n <td> ");
        strcat(webpage, directory[i].name);
        strcat(webpage, "</td>  <td>");
        
        if (directory[i].isdir == false ) strcat( webpage, "REGULAR FILE");
        else strcat (webpage, "DIRECTORY");

        strcat(webpage, "</td> \r\n </tr> \r\n");
    } 
  strcat(webpage, "</table> \r\n </body> \r\n </html>\r\n \r\n");
  return webpage;
}

int process_http_request(int httpsockfd)
{
  char reqbuf[MAXREQ];
  int n=0;
  /* IMPLEMENT ME!
     This services a single http request.  
     In this skeleton it responds to all connections by
     echoing the request to the client with a text/plain content type.
     This is handy for seeing what the requests look like, but is
     non-compliant.  It should do the following.

     Service the GET command.  
       - All other requests should receive a valid error 400 response.
       - A GET specifies a resource as the second token, followed by
       the http version.

     The directory in which the server runs should contain a directory
     ./www.  
     - Resources specified in the GET should be treated as a pathname 
     relative to that directory.
     - If the resource specifies a file, the file is returned to the
     client with an appropriate Content-Type in the response header.
     - If the resource specifies a directory and the directory contains
     a file named index.html, that file should be returned.
     - If no index.html is present, a html directory listing is returned
     - Paths must resolve to resources within the tree below files, i.e.,
     they cannot use .., ~, or / to reach to files outside.  Such 
     attempts should receive an error 404.
     - If the path does not resolve to a file with read access for
     'other' an error 404 is returned.
 */
  memset(reqbuf,0, MAXREQ);
  n = read(httpsockfd,reqbuf,MAXREQ-1);
  write(httpsockfd,textheader,strlen(textheader));
  write(httpsockfd,"You said:\n",10);
  write(STDOUT_FILENO, reqbuf, n );
  write(httpsockfd, reqbuf, n );
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