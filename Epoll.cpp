#include "Epoll.h"
#include <sys/epoll.h>
#include "Channel.h"
#include <vector>
#include "EventLoop.h"
#include <strings.h>

const int Epoll::INITVECTORSIZE = 16;
Epoll::Epoll(EventLoop *loop)
  : loop_(loop), epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
  events_(Epoll::INITVECTORSIZE)
{
}

Epoll::~Epoll()
{
  ::close(epollfd_);
}

void Epoll::removeChannel(Channel *channel)
{
  int fd = channel->fd();
  channels_.erase(fd);

  if(channel->status() == Channel::ADDED)
    _update(EPOLL_CTL_DEL, channel);

  channel->setStatus(Channel::NEWED);
}

void Epoll::updateChannel(Channel *channel)
{
  int status = channel->status();
  int fd = channel->fd();

  if(status == Channel::ADDED)
  {
    if(channel->isNonEvent())
      _update(EPOLL_CTL_DEL, channel);
    else
      _update(EPOLL_CTL_MOD, channel);
  }

  else if(status == Channel::DELETED)
  {
    channel->setStatus(Channel::ADDED);
    _update(EPOLL_CTL_ADD, channel);
  }

  else if(status == Channel::NEWED)
  {
    channels_[fd] = channel;
    _update(EPOLL_CTL_ADD, channel);
    channel->setStatus(Channel::ADDED);
  }

}

void Epoll::_update(int operation, Channel *channel)
{
  struct epoll_event event;
  bzero(&event, sizeof(event));
  event.data.ptr = channel;
  event.events = channel->events();

  int fd = channel->fd();

  //TODO 添加错误回滚
  epoll_ctl(epollfd_, operation, fd, &event);
}

void Epoll::fillActiveChannel(int num, std::vector<Channel*> *channelList)
{
  for(int i = 0; i < num; ++i)
  {
    Channel *channel = static_cast<Channel*>(events_[i].data.ptr);
    channel->setRespondEvents(events_[i].events);
    channelList->push_back(channel);
  }
}

void Epoll::poll(int timeOut, std::vector<Channel*> *channelList)
{
  int num = epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeOut);
  if(num > 0)
  {
    fillActiveChannel(num, channelList);
    if(static_cast<size_t>(num) == events_.size())
      events_.resize(events_.size() * 2);
  }
  else if(num == 0)
  {

  }
  else
  {
    // TODO 错误发生
  }
}

