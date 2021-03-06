#include "Acceptor.h"

Acceptor::Acceptor(EventLoop *loop, const InetAddress &inetAddress):
  loop_(loop), serviceSocket_(Socket::createFd()), serviceChannel_(loop, serviceSocket_.socketFd())
{
  Socket::bindFdAndAddress(serviceSocket_.socketFd(), inetAddress);
  serviceChannel_.setReadCallBack(std::bind(&Acceptor::_handleRead, this));
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

void Acceptor::_handleRead()
{
  InetAddress clientAddress;
  int connFd = serviceSocket_.acceptSocket(&clientAddress);
  if(connFd >= 0 && newConnectionCallBack_)
  {
    newConnectionCallBack_(connFd, clientAddress);
  }
  else
  {
    ::close(connFd);
  }
}




