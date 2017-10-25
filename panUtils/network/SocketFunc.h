#pragma once

const int E_SOCKET_ERROR = -1;
#ifdef _WIN32
const int E_SOCKET_WOULDBLOCK = WSAEWOULDBLOCK;
const int E_SOCKET_INTR = WSAEINTR;
const int E_SOCKET_AGAIN = WSAEWOULDBLOCK;
const int E_SOCKET_INPROGRESS = WSAEINPROGRESS;
#else
const int E_SOCKET_WOULDBLOCK = EWOULDBLOCK;
const int E_SOCKET_INTR = EINTR;
const int E_SOCKET_AGAIN = EAGAIN;
const int E_SOCKET_INPROGRESS = EINPROGRESS;
#endif

int SocketInit();
int SocketShutdown();
int SocketError();

int CloseSocket(int fd);

int SetSocketNoBlocking(int fd,bool block);

int SocketSend(int fd, const char *buf, int len,bool block=false);
int SocketRecv(int fd, char *buf, int len,bool block=false);