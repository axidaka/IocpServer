///////////////////////////////////////////////////////////
//  Network.cpp
//  Implementation of the Class Network
//  Created on:      14-一月-2008 16:31:54
//  Original author: Aitec
///////////////////////////////////////////////////////////
#include "stdafx.h"
#include "NetworkInterface.h" 

int NetworkInterface::InitNetwork()
{
	WSAData wsadata;
	int nRet = WSAStartup(MAKEWORD(2, 2), &wsadata);	

	return (NO_ERROR != nRet? RET_ERROR : RET_SUCCESS);
}

int NetworkInterface::DeinitNetwork()
{
	WSACleanup();

	return RET_SUCCESS;
}

int	NetworkInterface::MakeTCPServer(u_int16_t port)
{
	int					sockfd;
	struct sockaddr_in 	sin;

	memset(&sin,0,sizeof(sin));

	sin.sin_addr.s_addr     = INADDR_ANY;
	sin.sin_family          = AF_INET;
	sin.sin_port            = htons(port);

#ifdef __linux__
	if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0 )
#elif _WIN32
	if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET)
#else
	if((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0)
#endif
	{
		return RET_ERROR;
	}

	int	val = 1;
	//setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&val),sizeof(val));
	setsockopt(sockfd, SOL_SOCKET, SO_EXCLUSIVEADDRUSE, reinterpret_cast<char*>(&val),sizeof(val));
	
#ifdef __linux__
	if(bind(sockfd, reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)) < 0 )
#elif _WIN32
	if(bind(sockfd, reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)) == SOCKET_ERROR )
#else
	if(bind(sockfd, reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)) < 0 )
#endif
	{
		Close(sockfd);
		return RET_ERROR;
	}

	listen(sockfd,128);

	return sockfd;	
}

int	NetworkInterface::MakeTCPClient(const char *ipaddr,u_int16_t port)
{
	int		msec 	= 3000;
	bool 	cancel	= false;	
	return Connect1(ipaddr,port,msec,cancel);
}

int	NetworkInterface::MakeTCPClient(const char *ipaddr,u_int16_t port,bool const &cancel)
{
	int		msec 	= 3000;	
	return Connect1(ipaddr,port,msec,cancel);
}

int	NetworkInterface::MakeTCPClient(const char *ipaddr,u_int16_t port,int msec)
{
	bool 	cancel	= false;
	return Connect1(ipaddr,port,msec,cancel);
}

int NetworkInterface::MakeTCPClient(u_int32_t ipaddr,u_int16_t port)
{
	int		msec 	= 3000;
	bool 	cancel	= false;	
	return Connect1(ipaddr,port,msec,cancel);
}

int	NetworkInterface::MakeTCPClient(u_int32_t ipaddr,u_int16_t port,bool const &cancel)
{
	int		msec 	= 3000;	
	return Connect1(ipaddr,port,msec,cancel);
}

int	NetworkInterface::MakeTCPClient(u_int32_t ipaddr,u_int16_t port,int msec)
{
	bool 	cancel	= false;	
	return Connect1(ipaddr,port,msec,cancel);
}

int NetworkInterface::MakeAccept(int listenfd, struct sockaddr *saddr)
{
	int                 clifd;
	socklen_t           len;
	if ( !saddr )
	{
		return RET_ERROR;
	}

	len = sizeof(*saddr);
	if( (clifd = accept(listenfd, reinterpret_cast<struct sockaddr *>(saddr), &len)) < 0 )
	{
		return RET_ERROR;
	}

	return clifd;
}

int NetworkInterface::MakeAccept(int listenfd)
{
#ifdef __linux__
	struct sockaddr_un  saddr;
#elif _WIN32
	struct sockaddr_in  saddr;
#else
	struct sockaddr_un  saddr;
#endif 
	return MakeAccept(listenfd,reinterpret_cast<struct sockaddr *>(&saddr));
}

int	NetworkInterface::MakeUDPServer(u_int16_t port)
{
	int	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{ 
		return RET_ERROR;
	}
	
	int 	loop = 1;
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR,reinterpret_cast<char*>(&loop), sizeof(loop)) < 0)
	{
		Close(sockfd);
		return RET_ERROR;
	}

	struct sockaddr_in 	sin;
	memset(&sin, 0, sizeof(sin));

	sin.sin_family 			= AF_INET;
	sin.sin_addr.s_addr 	= htonl(INADDR_ANY);
	sin.sin_port 			= htons(port);
	if(bind(sockfd,reinterpret_cast<struct sockaddr *>(&sin), sizeof(sin)) < 0)
	{
		Close(sockfd);
		return RET_ERROR;
	}

	return sockfd;
}

int	NetworkInterface::MakeUDPClient()
{
	int	sockfd = socket(PF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0)
	{ 
		return RET_ERROR;
	}
	return sockfd;
}

#ifdef __linux__
int NetworkInterface::MakeUnixServer(const char *filename)
{
	int                 sockfd;
	struct sockaddr_un  saddr;
	int                 mode    = S_IRUSR|S_IWUSR|S_IRGRP|S_IWGRP|S_IROTH|S_IWOTH;

	bzero(&saddr, sizeof(saddr));

	if ( !filename || strlen(filename) > UNIX_PATH_MAX)
	{
		return RET_ERROR;
	}

	saddr.sun_family = AF_UNIX;
	strncpy(saddr.sun_path, filename, UNIX_PATH_MAX);

	unlink(filename);

	sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
	if(sockfd < 0)
	{
		return RET_ERROR;
	}

	if(bind(sockfd, reinterpret_cast<struct sockaddr*>(&saddr), SUN_LEN(&saddr)) != 0)
	{
		Close(sockfd);
		return RET_ERROR;
	}

	listen(sockfd, 128);

	chmod(filename,mode);

	return sockfd;
}

int NetworkInterface::MakeUnixClient(const char *filename)
{
	int			sockfd;
	sockaddr_un	saddr;

	if( (sockfd = socket(AF_LOCAL, SOCK_STREAM, 0)) < 0 )
	{
		return RET_ERROR;
	}

	bzero(&saddr, sizeof(struct sockaddr_un));

	saddr.sun_family = AF_UNIX;
	strncpy(saddr.sun_path, filename, UNIX_PATH_MAX);

	if (connect(sockfd, reinterpret_cast<struct sockaddr*>(&saddr), sizeof(saddr)))
	{
		Close(sockfd);
		return RET_ERROR;
	}

	return sockfd;
}
#endif

int NetworkInterface::Select(int nfds,fd_set *readfds,fd_set *writefds,fd_set *exceptfds,struct timeval *tv)
{
	while(true)
	{
		int nselect = select(nfds, readfds, writefds, exceptfds, tv); 
#ifdef __linux__	
		if(nselect < 0 && errno == EINTR) 
#elif _WIN32
		if (false)
#else 
		if (nselect < 0 && errno == EINTR) 
#endif
		{
			continue;
		}
		
		return nselect;
	}
}

int NetworkInterface::Close(int sockfd)
{
#ifdef  __linux__
	close(sockfd);
#elif	_WIN32
	closesocket(sockfd);
#else
	close(sockfd);
#endif
	return RET_SUCCESS;
}

ssize_t NetworkInterface::Recvn(int fd, void* vptr, ssize_t n)
{
	ssize_t  nleft;
	ssize_t  nread;
	char	*ptr;
	fd_set  fds;
	struct  timeval tv;

	ptr     = reinterpret_cast<char*>(vptr);
	nleft   = n; 
	while(nleft > 0)
	{
#ifdef __linux__
		nread = read(fd,ptr,nleft);
#elif _WIN32
		nread = recv(fd,ptr,nleft,0);
#else
		nread = read(fd,ptr,nleft); 
#endif

#ifdef __linux__
		if(nread < 0 )
#elif _WIN32
		if(nread == SOCKET_ERROR )
#else
		if(nread < 0 )
#endif
		{ 
#ifdef __linux__
			if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) 
			{
				return RET_ERROR;
			} 
#elif _WIN32
			int err = GetLastError();
			if(err != WSAEWOULDBLOCK)
			{ 
				return RET_ERROR;
			}
#else 
			if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) 
			{
				return RET_ERROR;
			} 
#endif

			FD_ZERO(&fds);
			FD_SET(fd, &fds); 
			tv.tv_sec  = 5;
			tv.tv_usec = 0;
			int nselect = Select(fd+ 1, &fds, NULL, NULL, &tv); 
			if(nselect <= 0)
			{
				return RET_ERROR;
			}
		}
		else if (nread == 0)
		{ 
			return RET_ERROR;
		}
		else
		{
			nleft -= nread;
			ptr	  += nread; 
		} 
	} 
	return (n - nleft);
}


ssize_t	NetworkInterface::Sendn(int fd, const void* vptr, ssize_t n)
{
	ssize_t     nleft;
	ssize_t     nwritten;
	const char *ptr;

	fd_set fds;
	struct timeval tv;

	ptr     = reinterpret_cast<const char*>(vptr);
	nleft   = n;

	while(nleft > 0)
	{
#ifdef __linux__
		nwritten = write(fd,ptr,nleft); 
#elif _WIN32
		nwritten = send(fd,ptr,nleft,0);
#else
		nwritten = write(fd,ptr,nleft); 
#endif

#ifdef __linux__
		if(nwritten < 0 )
#elif _WIN32
		if(nwritten == SOCKET_ERROR )
#else
		if(nwritten < 0 )
#endif
		{ 
#ifdef __linux__
			if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) 
			{
				return RET_ERROR;
			} 
#elif _WIN32
			int err = GetLastError();
			if(err != WSAEWOULDBLOCK)
			{ 
				return RET_ERROR;
			}
#else 
			if (errno != EWOULDBLOCK && errno != EAGAIN && errno != EINTR) 
			{
				return RET_ERROR;
			} 
#endif

			FD_ZERO(&fds);
			FD_SET(fd, &fds); 
			tv.tv_sec  = 5;
			tv.tv_usec = 0;
			
			int nselect = Select(fd + 1,NULL,&fds,NULL,&tv);
			if(nselect <= 0)
			{
				return RET_ERROR;
			}
		}
		else if(nwritten == 0)
		{ 
			return RET_ERROR;
		}
		else 
		{
			nleft   -= nwritten;
			ptr     += nwritten;
		} 
	} 
	return (n-nleft);
} 

ssize_t	NetworkInterface::SendTo(int sockfd,const void *vptr,ssize_t n,int flags,const struct sockaddr *addr,socklen_t len)
{
	return sendto(sockfd,reinterpret_cast<const char*>(vptr),n,flags,addr,len);
}

ssize_t	NetworkInterface::RecvFrom(int sockfd,void *vptr,ssize_t n,int flags,struct sockaddr *addr,socklen_t *len)
{
	return recvfrom(sockfd,reinterpret_cast<char*>(vptr),n,flags,addr,len);
} 

ssize_t NetworkInterface::RecvFrom(int sockfd, void *vptr, ssize_t n, int flags)
{
	struct sockaddr addr;
	socklen_t len = sizeof(struct sockaddr);
	return recvfrom(sockfd,reinterpret_cast<char*>(vptr),n,flags,&addr,&len);
}

int	NetworkInterface::JoinMultiCast(int sockfd,struct ip_mreq *command)
{
	int 	loop = 1;
	if(setsockopt(sockfd, IPPROTO_IP, IP_MULTICAST_LOOP, reinterpret_cast<char*>(&loop), sizeof(loop)) < 0)
	{
		return RET_ERROR;
	}
	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, reinterpret_cast<char*>(command), sizeof(*command)) < 0)
	{
		return RET_ERROR;
	}
	return RET_SUCCESS;	
}

int	NetworkInterface::QuitMultiCast(int sockfd,struct ip_mreq *command)
{
	if(setsockopt(sockfd, IPPROTO_IP, IP_DROP_MEMBERSHIP, reinterpret_cast<char*>(command), sizeof(*command)) < 0)
	{
		return RET_ERROR;
	}	
	return RET_SUCCESS;	
}

int NetworkInterface::SetNonBlock(int sockfd)
{
#ifdef __linux__
    int flags; 
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
    { 
		return RET_ERROR;
    } 
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) 
    { 
		return RET_ERROR;
    } 
	return RET_SUCCESS;
#elif _WIN32
	u_long nonblock = 1;
	if(ioctlsocket(sockfd,FIONBIO,&nonblock) == 0) 
	{
		return RET_SUCCESS;
	}
	else
	{
		return RET_ERROR;
	}
#else
    int flags; 
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
    { 
		return RET_ERROR;
    } 
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) < 0) 
    { 
		return RET_ERROR;
    } 
	return RET_SUCCESS; 
#endif
}

int NetworkInterface::SetBlock(int sockfd)
{
#ifdef __linux__
    int flags; 
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
    { 
		return RET_ERROR;
    } 
    if (fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK)) < 0) 
    { 
		return RET_ERROR;
    } 
	return RET_SUCCESS;
#elif	_WIN32
	u_long block = 0;
	if(ioctlsocket(sockfd,FIONBIO,&block) == 0) 
	{
		return RET_SUCCESS;
	}
	else
	{
		return RET_ERROR;
	}

#else
    int flags; 
    if ((flags = fcntl(sockfd, F_GETFL, 0)) < 0) 
    { 
		return RET_ERROR;
    } 
    if (fcntl(sockfd, F_SETFL, flags & (~O_NONBLOCK)) < 0) 
    { 
		return RET_ERROR;
    } 
	return RET_SUCCESS; 
#endif
}

#ifdef _WIN32
#include "mstcpip.h"
#endif

int NetworkInterface::SetKeepLive(int sockfd,int idle,int intvl,int cnt)
{ 
	int opt = 1;
	if (setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,reinterpret_cast<char*>(&opt),sizeof(opt)) < 0)
	{
		return RET_ERROR;
	} 

#ifdef __linux__
	if (setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPIDLE, &idle, sizeof(idle)) < 0 ||
	    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPINTVL,&intvl,sizeof(intvl))< 0 ||
	    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPCNT,  &cnt,  sizeof(cnt)) < 0)
	{
		return RET_ERROR;
	} 
#elif _WIN32
	unsigned long dw;
	tcp_keepalive live,liveout;

	live.onoff				=	1;
	live.keepalivetime		= idle * 1000;	
	live.keepaliveinterval	= intvl * 1000;

	if(WSAIoctl(sockfd,SIO_KEEPALIVE_VALS,&live,sizeof(live),&liveout,sizeof(liveout),&dw,NULL,NULL) < 0)
	{ 
		return RET_ERROR;
	} 
#else
	if (setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPIDLE, &idle, sizeof(idle)) < 0 ||
	    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPINTVL,&intvl,sizeof(intvl))< 0 ||
	    setsockopt(sockfd,IPPROTO_TCP,TCP_KEEPCNT,  &cnt,  sizeof(cnt)) < 0)
	{
		return RET_ERROR;
	} 
#endif

	return RET_SUCCESS;
}

int NetworkInterface::SetNonKeepLive(int sockfd)
{ 
	int opt = 0;
	if (setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,reinterpret_cast<char*>(&opt),sizeof(opt)) < 0)
	{
		return RET_ERROR;
	} 
	return RET_SUCCESS;
}

int NetworkInterface::SetSendBuf(int sockfd, int Size)
{
	int opt = Size;

	if(setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, reinterpret_cast<char*>(&opt), sizeof(opt)) < 0)
	{
		return RET_ERROR;
	}
	return RET_SUCCESS;
}

int NetworkInterface::SetRecvBuf(int sockfd, int Size)
{
	int opt = Size;

	if(setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, reinterpret_cast<char*>(&opt), sizeof(opt)) < 0)
	{
		return RET_ERROR;
	}
	return RET_SUCCESS;
}

int NetworkInterface::SetNonDelay(int sockfd)
{ 
	int opt = 1;

	if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,reinterpret_cast<char*>(&opt),sizeof(opt)) < 0)
	{
		return RET_ERROR;
	}

	return RET_SUCCESS; 
}

int NetworkInterface::SetDelay(int sockfd)
{
	int opt = 0;

	if(setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<char*>(&opt),sizeof(opt)) < 0)
	{
		return RET_ERROR;
	}

	return RET_SUCCESS; 
}


int NetworkInterface::SetLinger(int sockfd)
{
#ifdef _WIN32
	linger opt;
	opt.l_onoff = 1;  // (在closesocket()调用,但是还有数据没发送完毕的时候容许逗留)
	opt.l_linger = 0; // (容许逗留的时间为0秒)
	if(setsockopt(sockfd, SOL_SOCKET, SO_LINGER, (const char*)&opt, sizeof(linger)) < 0)
	{
		return RET_ERROR;
	}

	return RET_SUCCESS; 
#else
	return RET_ERROR;
#endif
}


int NetworkInterface::Connect1(const char* ipaddr,u_int16_t port,int msec,const bool &cancel)
{
	return Connect2(inet_addr(ipaddr),htons(port),msec,cancel);
}

int NetworkInterface::Connect1(u_int32_t ipaddr,u_int16_t port,int msec,const bool &cancel)
{
	return Connect2(htonl(ipaddr),htons(port),msec,cancel);
}

int NetworkInterface::Connect2(u_int32_t ipaddr,u_int16_t port,int msec,const bool &cancel)
{
	struct 	sockaddr_in	addr;
	int	   	sockfd;

	memset(&addr,0,sizeof(addr));

	addr.sin_addr.s_addr	= ipaddr;
	addr.sin_family			= AF_INET;	
	addr.sin_port 			= port;
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) < 0 )
	{
		return RET_ERROR;
	} 

	if(Connect3(sockfd,reinterpret_cast<struct sockaddr *>(&addr), sizeof(addr), msec, cancel) < 0)
	{
		NetworkInterface::Close(sockfd);
		return RET_ERROR;
	}
	else
	{
		return sockfd;
	}
}

int NetworkInterface::Connect3(int sockfd, const struct sockaddr * saptr, socklen_t salen, int msec, const bool & cancel)
{
	int		n;
	int		unit = 10;

	SetNonBlock(sockfd);

#ifdef __linux__
	if((n = connect(sockfd, saptr, salen)) < 0 )
#elif  _WIN32
	if((n = connect(sockfd, saptr, salen)) == SOCKET_ERROR )
#else
	if((n = connect(sockfd, saptr, salen)) < 0 )
#endif
	{
#ifdef __linux__
		if(errno != EINPROGRESS)
		{
			goto done;
		}
#elif _WIN32
		int err = GetLastError();
		if(err != WSAEWOULDBLOCK)
		{
			goto done;
		} 
#else 
		if(errno != EINPROGRESS)
		{
			goto done;
		} 
#endif
	}

	if(n == 0)
	{
		SetBlock(sockfd);
		return RET_SUCCESS;
	}

	while( (msec = msec - unit) >= 0 && !cancel )
	{
		struct	timeval tval;
		fd_set			rset,wset;

		FD_ZERO(&rset);
		FD_ZERO(&wset);
		FD_SET(sockfd, &rset);
		FD_SET(sockfd,&wset);

		tval.tv_sec		= 0;
		tval.tv_usec	= unit * 1000;
		if( (n = NetworkInterface::Select(sockfd + 1, &rset, &wset, NULL, &tval)) <= 0)
		{
			continue;
		}

		if(FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset))
		{
			int			error	= 0;
			socklen_t 	len		= sizeof(error);
			if(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char*)&error, &len) < 0 || error)
			{
				goto done;
			}

			SetBlock(sockfd);
			return RET_SUCCESS;
		}
		else
		{
			goto done;
		}
	} 

done:
	SetBlock(sockfd); 
	return RET_ERROR;
}

int	NetworkInterface::MakeInetServer(u_int16_t port)
{
	return MakeTCPServer(port);
}

int	NetworkInterface::MakeInetClient(char* ipaddr, u_int16_t port)
{
	return MakeTCPClient(ipaddr,port);
}

int	NetworkInterface::MakeInetClient(char* ipaddr, u_int16_t port, int const & cancel)
{ 
	int		sockfd;
	int		count	= 300;
	int 	msec	= 10;

	while(count-- > 0)
	{ 
		if(cancel == 0)
		{
			if((sockfd = NetworkInterface::MakeTCPClient(ipaddr,port,msec)) < 0)
			{
				continue;
			}
			else
			{
				return sockfd;
			}
		}
		else
		{
			return RET_ERROR;
		}
	} 

	return RET_ERROR;
}

int	NetworkInterface::MakeInetClientFast(char* ipaddr, u_int16_t port, int msec)
{
	return MakeTCPClient(ipaddr,port,msec); 
}

ssize_t	NetworkInterface::Readn(int fd, void* vptr, ssize_t n)
{
	return Recvn(fd,vptr,n);
}

ssize_t	NetworkInterface::Writen(int fd, void* vptr, ssize_t n)
{
	return Sendn(fd,vptr,n);
}

