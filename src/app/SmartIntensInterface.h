
#if !defined(SMART_INTENS_INTERFACE_H)
#define SMART_INTENS_INTERFACE_H

#include <string>
#include <vector>

/**
Smart Intens Interface definition
*/
class SmartIntensInterface {

 public:

  /** publish class TransferData */
  class TransferData {
  public:
  TransferData(const std::string& h)
    : header(h) {
    }
  TransferData(const std::string& h, const std::string& d)
    : header(h) {
      data.push_back(d);
    }
  TransferData(const std::string& h, const std::vector<std::string>& d)
    : header(h), data(d) {
    }
    const std::string& getHeader() const { return header; }
    const std::vector<std::string>& getData()   const { return data; }
  private:
    const std::string header;
    std::vector<std::string> data;
  };

  /** publish methods */
  virtual void subscribedData(const TransferData& pData) = 0;
};

#endif
