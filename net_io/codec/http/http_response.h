
#ifndef _NET_HTTP_RESPONSE_MESSAGE_H
#define _NET_HTTP_RESPONSE_MESSAGE_H

#include <map>
#include <sstream>

#include <net_io/codec/codec_message.h>

namespace lt {
namespace net {

class HttpResponse;

typedef std::map<std::string, std::string> KeyValMap;
typedef std::shared_ptr<HttpResponse> RefHttpResponse;

class HttpResponse : public CodecMessage {
public:
  HttpResponse();
  ~HttpResponse();

  static RefHttpResponse CreateWithCode(uint16_t code);

  std::string& MutableBody();
  const std::string& Body() const;

  KeyValMap& MutableHeaders();

  const KeyValMap& Headers() const;
  bool HasHeaderField(const std::string) const;
  void InsertHeader(const std::string&, const std::string&);
  void InsertHeader(const char*, const char*);
  const std::string& GetHeader(const std::string&) const;

  bool IsKeepAlive() const;
  void SetKeepAlive(bool alive);

  const std::string Dump() const override;

  uint16_t ResponseCode() const;
  void SetResponseCode(uint16_t code);
  std::string StatusCodeInfo() const;
  int VersionMajor() const {return http_major_;}
  int VersionMinor() const {return http_minor_;}
private:
  friend class HttpCodecService;
  friend class ResParseContext;

  bool keepalive_;
  uint8_t http_major_;
  uint8_t http_minor_;
  uint16_t status_code_;

  std::string body_;
  KeyValMap headers_;
};


}}
#endif
