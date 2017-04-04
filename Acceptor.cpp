#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, InetAddress inetAddress):
  loop_(loop), serviceSocket_(Socket::create()), serviceChannel_(loop, serviceSocket_.socketFd())
{
  Socket::bindFdAndAddress(serviceSocket_, inetAddress);
  serviceChannel_.setReadCallBack(std::bind(&Acceptor::handleRead, this));
}

Acceptor::~Acceptor()
{
  serviceChannel_.disableAll();
  serviceChannel_.remove();
}

void Acceptor::listen()
{
  serviceSocket_.listen();
  serviceChannel_.enableReading();
}

void Acceptor::handleRead()
{
  InetAddress clientAddress;
  int connFd = serviceSocket_.acceptSocket(clientAddress);
  if(connFd >= && newConnectionCallBack)
  {
    newConnectionCallBack(connFd, clientAddress);
  }
  else
  {
    ::close(connFd);
  }
}



