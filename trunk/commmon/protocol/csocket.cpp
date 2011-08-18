
#include "csocket.h"

#define PDEBUG 0

/**
 * @param iPort local listenning port
 * @throws CSocketException is server socket could not be created
 */
CSocket::CSocket( int iPort)
{
	Reset( iPort);

	if( bind( m_Socket, ( SOCKADDR *)&m_sockaddr, sizeof( sockaddr)) != 0)
		throw CSocketException( "bind() failed");

	if( listen( m_Socket, 0) != 0)
		throw CSocketException( "accept() failed");
}

/**
 * @param szRemoteAddr Remote Machine Address
 * @param iRemotePort Server Listenning Port
 * @throws CSocketException if client socket could not be created
 */
CSocket::CSocket( char *szRemoteAddr, int iPort)
{
	if( !szRemoteAddr)
		throw CSocketException( "Invalid parameters");

	Reset( iPort);

	// first guess => try to resolve it as IP@
	m_sockaddr.sin_addr.s_addr = inet_addr( szRemoteAddr);
	if( m_sockaddr.sin_addr.s_addr == INADDR_NONE)
	{	// screwed => try to resolve it as name
	LPHOSTENT lpHost = gethostbyname( szRemoteAddr);
		if( !lpHost)
			throw CSocketException( "Unable to solve this address");
		m_sockaddr.sin_addr.s_addr = **(int**)(lpHost->h_addr_list);
	}

	// actually performs connection
	if( connect( m_Socket, ( SOCKADDR*)&m_sockaddr, sizeof( sockaddr)) != 0)
		throw CSocketException( "connect() failed");
}

/**
 * Create a socket for data transfer (typically after Accept)
 * @param Socket the socket descriptor for this new object
 */
CSocket* CSocket::make_socket( SOCKET Socket)
{
  CSocket* ptr = new CSocket;
  ptr->m_Socket = Socket;
  return ptr;
}

/**
 * Destructor
 */
CSocket::~CSocket()
{
	Close();
}

/**
 * Wait for incomming connections on server socket
 * @return CSocket new data socket for this incomming client. Can be NULL is anything went wrong
 */
CSocket * CSocket::Accept()
{

socklen_t nlen = sizeof( sockaddr);
SOCKET Socket = accept( m_Socket, ( SOCKADDR *)&m_sockaddr, &nlen);

	if( Socket == -1)
		return( NULL);

	return( make_socket( Socket));
}


/**
 * Close current socket
 */
void CSocket::Close()
{
  if( m_Socket != (~0))
#if WIN_SERV
		closesocket( m_Socket);
#elif LIN_SERV
		close( m_Socket);
#endif
}

/**
 * Read data available in socket or waits for incomming informations
 * @param pData Buffer where informations will be stored
 * @param iLen Max length of incomming data
 * @return Number of bytes read or -1 if anything went wrong
 */
int CSocket::Read( void * pData, unsigned int iLen)
{
#if PDEBUG
	printf("asking for %d\n", iLen);
#endif
	if( !pData || !iLen)
		return( -1);

	int ret = recv( m_Socket, ( char *)pData, iLen, 0);
#if PDEBUG
	printf("Read %d\n", ret);
#endif
	return ret;
}

/**
 * Initialisation common to all constructors
 */
void CSocket::Reset( unsigned int iPort)
{
#if WIN_SERV
	// Initialize winsock
	if( WSAStartup( MAKEWORD(2,0), &m_WSAData) != 0)
		throw CSocketException( "WSAStartup() failed");
#endif
	// Actually create the socket
	m_Socket = socket( PF_INET, SOCK_STREAM, IPPROTO_TCP);
	if( m_Socket == (~0))
		throw CSocketException( "socket() failed");

	// sockaddr initialisation
	memset( &m_sockaddr, 0, sizeof( sockaddr));

	m_sockaddr.sin_family = AF_INET;
	m_sockaddr.sin_port = htons( iPort);
	m_sockaddr.sin_addr.s_addr = INADDR_ANY;
}

/**
 * @param pData Buffer to be sent
 * @param iSize Number of bytes to be sent from buffer
 * @return the number of sent bytes or -1 if anything went wrong
 */
int CSocket::Write( void * pData, unsigned int iSize)
{
	if( !pData || !iSize)
		return( -1);

	int ret = ( int)send( m_Socket, ( char*)pData, iSize, 0);
#if PDEBUG
	printf("Wrote %d\n", ret);
#endif
	return ret;
}

int CSocket::Peek()
{
	fd_set fds;
	struct timeval tv;

	// sock is an intialized socket handle
	tv.tv_sec = 2000000;
	tv.tv_usec = 500000;
	// tv now represents 2.5 seconds

	FD_ZERO(&fds);
	FD_SET(m_Socket, &fds); // adds sock to the file descriptor set

	select(m_Socket+1, &fds, NULL, NULL, &tv);
	if (FD_ISSET(m_Socket, &fds))
		return 1;
	else	
		return 0;
}
