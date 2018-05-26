///////////////////////////////////////////////////////////
//  Network.h
//  Implementation of the Class Network
//  Created on:      14-Ò»ÔÂ-2008 16:31:53
//  Original author: Aitec
///////////////////////////////////////////////////////////

#if !defined(EA_76114261_477A_4b62_88C9_C98E8452015D__INCLUDED_)
#define EA_76114261_477A_4b62_88C9_C98E8452015D__INCLUDED_

#include <stdio.h>
#include <stdlib.h>
#include <string.h> 

#ifdef __linux__
	#include <unistd.h>
	#include <errno.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/select.h>
	#include <sys/un.h> 
#elif  _WIN32				/*** win32 */ 
	#include <Winsock2.h> 
	#include <ws2tcpip.h>
	#define ssize_t			unsigned int
	#define u_int8_t		unsigned char
	#define u_int16_t		unsigned short
	#define u_int32_t		unsigned int
	#define socklen_t		int
#else						/*** other os */
	#include <unistd.h>
	#include <errno.h>
	#include <netinet/tcp.h>
	#include <arpa/inet.h>
	#include <fcntl.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <sys/types.h> 
	#include <sys/select.h>
	#include <sys/un.h>
#endif 


#ifndef UNIX_PATH_MAX
#define UNIX_PATH_MAX 108

#endif

#ifndef RET_SUCCESS
#define RET_SUCCESS	0
#endif

#ifndef RET_ERROR
#define RET_ERROR 	-1
#endif

class NetworkInterface
{ 
public:
	
	// Init
	static  int     InitNetwork();
	static  int     DeinitNetwork();
public:
	// tcp
	static	int		MakeTCPServer(u_int16_t port);
	static	int		MakeTCPClient(const char *ipaddr,u_int16_t port);
	static	int		MakeTCPClient(const char *ipaddr,u_int16_t port,bool const &cancel);
	static	int		MakeTCPClient(const char *ipaddr,u_int16_t port,int msec);
	static	int 	MakeTCPClient(u_int32_t ipaddr,u_int16_t port);
	static	int		MakeTCPClient(u_int32_t ipaddr,u_int16_t port,bool const &cancel);
	static	int		MakeTCPClient(u_int32_t ipaddr,u_int16_t port,int msec);
public:
	// udp
	static	int		MakeUDPServer(u_int16_t port);
	static	int		MakeUDPClient();
#ifdef __linux__
public:
	// unix
	static	int		MakeUnixServer(const char *filename);
	static	int		MakeUnixClient(const char *filename);
#endif
public:
	// tcp and unix 
	static	int 	MakeAccept(int listenfd);
	static	int		MakeAccept(int listenfd,struct sockaddr *saddr);
public:
	// tcp¡¢udp and unix
	static	int		Select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *tv);
public:
	// tcp and unix io
	static 	ssize_t	Sendn(int fd,const void *vptr,ssize_t n);
	static 	ssize_t	Recvn(int fd,void *vptr,ssize_t n);
public:
	// udp io
	static	ssize_t	SendTo	(int sockfd,const void *vptr,ssize_t n,int flags,const struct sockaddr *addr,socklen_t len);
	static	ssize_t	RecvFrom(int sockfd,void *vptr,ssize_t n,int flags,struct sockaddr *addr,socklen_t *len);
	static	ssize_t	RecvFrom(int sockfd,void *vptr,ssize_t n, int flags);
public:
	// tcp¡¢udp and unix
	static	int		Close(int sockfd);
public:
	// udp
	static	int		JoinMultiCast(int sockfd,struct ip_mreq *command);
	static	int		QuitMultiCast(int sockfd,struct ip_mreq *command);
public:	
	// tcp¡¢udp and unix
	static 	int 	SetNonBlock(int sockfd);
	static 	int 	SetBlock(int sockfd);
public:	
	// tcp and unix
	static 	int 	SetNonKeepLive(int sockfd);
   	static 	int 	SetKeepLive(int sockfd,int idle,int intvl,int cnt);
public:
	// tcp¡¢udp and unix
	static 	int 	SetSendBuf(int sockfd, int Size);
	static 	int 	SetRecvBuf(int sockfd, int Size);
public:
	// tcp and unix
	static 	int 	SetNonDelay(int sockfd);
	static 	int 	SetDelay(int sockfd);

public:
	//tcp 
	static int		SetLinger(int sockfd);
private:
	static int 		Connect1(const char* ipaddr,u_int16_t port,int msec,const bool &cancel);
	static int 		Connect1(u_int32_t ipaddr,u_int16_t port,int msec,const bool &cancel);
	static int 		Connect2(u_int32_t ipaddr,u_int16_t port,int msec,const bool &cancel);
	static int 		Connect3(int sockfd, const struct sockaddr * saptr, socklen_t salen, int msec, const bool & cancel);
public:
	static int		MakeInetServer(u_int16_t port);	
	static int		MakeInetClient(char* ipaddr, u_int16_t port);
	static int		MakeInetClient(char* ipaddr, u_int16_t port, int const & cancel); 
	static int 		MakeInetClientFast(char* ipaddr, u_int16_t port, int waitmilliseconds);
	static ssize_t	Readn(int fd, void* vptr, ssize_t n);
	static ssize_t	Writen(int fd, void* vptr, ssize_t n); 
};
#endif // !defined(EA_76114261_477A_4b62_88C9_C98E8452015D__INCLUDED_)
