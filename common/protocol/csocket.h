#ifndef __CSOCKET_H
#define __CSOCKET_H

#include "machine.h"
#if WIN_SERV
#include "stdio.h"
#include "winsock2.h"
#elif LIN_SERV       
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <netdb.h>
using namespace std;

typedef int SOCKET;
typedef   struct  sockaddr WSADATA;
typedef   struct  sockaddr_in SOCKADDR_IN;
typedef   struct  sockaddr SOCKADDR;
typedef   struct  hostent *LPHOSTENT;
#endif

class CSocket
{
public:
	CSocket( char *szRemoteAddr, int iPort);
	CSocket( int iPort);
	CSocket() {};

	CSocket* make_socket( SOCKET Socket);
	~CSocket();

	CSocket * Accept( void);
	void Close( void);
	int Read( void * pData, unsigned int iLen);
	int Write( void * pData, unsigned int iLen);

	int Peek();
	
	SOCKET get_socket() {return m_Socket;}
private:
	SOCKET m_Socket;
	WSADATA m_WSAData;
	SOCKADDR_IN m_sockaddr;

	void Reset( unsigned int iPort);
};

class CSocketException
{
public:
	CSocketException( char * szText)
	{
		strcpy( m_szText, szText);
	}

	~CSocketException(){};

	char * getText(){ return( m_szText);}

private:
	char m_szText[ 128];
};

#endif //__CSOCKET_H
