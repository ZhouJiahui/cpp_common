#ifndef CPPLY_QPS_METER_H_
#define CPPLY_QPS_METER_H_

#include <stdint.h>
#include <string>

namespace cpply {

class QpsMeter {
public:
  QpsMeter(size_t report_interval_sec = 5);

  bool Action(int n_action, std::string& r);

  void Reset();

  void SetReportInterval(size_t report_interval_sec);

  void Report(std::string& r);

private:
  int64_t Now();
private:
  int64_t n_action_;
  int64_t last_us_;
  int64_t report_interval_us_;
};

}

#endif // CPPLY_QPS_METER_H_
