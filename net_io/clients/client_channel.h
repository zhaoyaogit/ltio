#ifndef _LT_NET_CLIENT_CHANNEL_H
#define _LT_NET_CLIENT_CHANNEL_H

#include "net_io/url_utils.h"
#include "net_io/net_callback.h"
#include "net_io/codec/codec_message.h"
#include "net_io/codec/codec_service.h"

#include "client_base.h"

namespace lt {
namespace net {

class ClientChannel;

REF_TYPEDEFINE(ClientChannel);

class ClientChannel : public CodecService::Delegate {
public:
  class Delegate {
  public:
    virtual const ClientConfig& GetClientConfig() const = 0;
    virtual const url::RemoteInfo& GetRemoteInfo() const = 0;

    virtual void OnClientChannelInited(const ClientChannel* channel) = 0;
    virtual void OnClientChannelClosed(const RefClientChannel& channel) = 0;
    virtual void OnRequestGetResponse(const RefCodecMessage&, const RefCodecMessage&) = 0;
  };

  enum State {
    kInitialing   = 0,
    kReady        = 1,
    kClosing      = 2,
    kDisconnected = 3
  };

  ClientChannel(Delegate* d, const RefCodecService& service);
  virtual ~ClientChannel();

  virtual void StartClientChannel();
  virtual void CloseClientChannel();
  // a change for close all inprogress request
  virtual void BeforeCloseChannel() = 0;
  virtual void SendRequest(RefCodecMessage request) = 0;

  bool Ready() const {return state_ == kReady;}
  bool Closing() const {return state_ == kClosing;}
  bool Initializing() const {return state_ == kInitialing;}

  void SetRequestTimeout(uint32_t ms) {request_timeout_ = ms;};
  base::EventPump* EventPump() {return codec_->Pump();}
  base::MessageLoop* IOLoop() {return codec_->IOLoop();};

  //override from CodecService::Delegate
  const url::RemoteInfo* GetRemoteInfo() const override;
  void OnProtocolServiceGone(const RefCodecService& service) override;
  void OnProtocolServiceReady(const RefCodecService& service) override;
protected:
  void OnHearbeatTimerInvoke();
  std::string ConnectionInfo() const;

  // return true when message be handled, otherwise return false
  bool HandleResponse(const RefCodecMessage& req,
                      const RefCodecMessage& res);
protected:
  Delegate* delegate_;
  State state_ = kInitialing;
  RefCodecService codec_;
  uint32_t request_timeout_ = 5000; //5s
  base::TimeoutEvent* heartbeat_timer_ = NULL;
  RefCodecMessage heartbeat_message_;
};

RefClientChannel CreateClientChannel(ClientChannel::Delegate*, RefCodecService);

}}//end namespace
#endif
