#include "qps_meter.h"
#include <sys/time.h>

namespace cpply {

QpsMeter::QpsMeter(size_t report_interval_sec)
  : report_interval_us_(report_interval_sec * 1000 * 1000) {
    Reset();
}

bool QpsMeter::Action(int n_action, std::string& r) {
  n_action_ += n_action;
  int64_t now = Now();
  if (now - last_us_ < report_interval_us_) {
    return false;
  }
  Report(r);
  Reset();
  return true;
} 

void QpsMeter::Reset() {
  last_us_ = Now();
  n_action_ = 0;
}

void QpsMeter::SetReportInterval(size_t report_interval_sec) {
  report_interval_us_ = report_interval_sec * 1000 * 1000;
  Reset();
}

void QpsMeter::Report(std::string& r) {
  char buf[128];
  int64_t now = Now();
  int64_t tm_us = now - last_us_;
  int qps = 0;
  if (tm_us != 0) {

  }
  snprintf(buf, sizeof(buf), "qps=%d, ms=%.3f", qps, tm_us / 1000.0);
  r = buf;
}

int64_t QpsMeter::Now() {
  struct timeval now;
  gettimeofday(&now, nullptr);
  int64_t us = now.tv_sec * 1000 * 1000 + now.tv_usec;
  return us;
}

}
