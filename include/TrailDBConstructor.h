#ifndef CPP_TRAILDB_CONSTRUCTOR_H_INCLUDED
#define CPP_TRAILDB_CONSTRUCTOR_H_INCLUDED

#include <iostream>
#include <iomanip>
#include <vector>
#include <map>
#include <exception>
#include <algorithm>
#include <functional>
#include <sstream>
#include <string>
#include <cstdint>
#include <fstream>

#include "TrailDBcpp.h"
#include "TrailDBException.h"
#include "ByteManipulations.h"

extern "C" {
#ifndef BAZEL
#include <traildb.h>
#else
#include "traildb/traildb.h"
#endif
}

class TrailDBConstructor {
 public:
  TrailDBConstructor(std::string path, std::vector<std::string>& fields);
  ~TrailDBConstructor();
  void Finalize();
  void Append(TrailDB tdb);
  void Add(std::string cookie, std::uint64_t timestamp,
           const std::vector<std::string>& values);
  void RawCookie(std::string cookie, std::uint8_t res[16]);

 private:
  tdb_cons* cons_;

  bool finalized_;
  std::string opath_;
  std::vector<std::string> ofields_;
  static const unsigned TDB_KEY_SIZE_BYTES = 16;
};

TrailDBConstructor::TrailDBConstructor(std::string path,
                                       std::vector<std::string>& fields)
    : finalized_(false), opath_(path), ofields_(fields) {
  std::vector<const char*> ofield_names;
  for (auto& s : ofields_) {
    ofield_names.push_back(&s.front());
  }
  const char** cofield_names = &ofield_names.front();

  cons_ = tdb_cons_init();

  if (tdb_cons_open(cons_, opath_.c_str(), cofield_names, ofields_.size())) {
    throw TrailDBException();
  }
};

TrailDBConstructor::~TrailDBConstructor() { tdb_cons_close(cons_); };

void TrailDBConstructor::Finalize() {
  if (!finalized_) {
    if (tdb_cons_finalize(cons_)) {
      throw TrailDBException();
    }
    finalized_ = true;
  }
}

void TrailDBConstructor::Append(TrailDB tdb) {
  if (tdb_cons_append(cons_, tdb.GetDB())) {
    throw TrailDBException();
  }
}

void TrailDBConstructor::Add(const std::string hexuuid,
                             const std::uint64_t timestamp,
                             const std::vector<std::string>& values) {
  std::uint8_t uuid[16];
  const std::uint8_t* hex_str =
      reinterpret_cast<const std::uint8_t*>(hexuuid.c_str());
  tdb_uuid_raw(hex_str, uuid);

  std::vector<const char*> val_entries;
  std::vector<std::uint64_t> val_len;
  for (auto& s : values) {
    val_entries.push_back(&s.front());
    val_len.push_back(s.size());
  }
  const char** cvalues_entries = &val_entries.front();
  const std::uint64_t* cval_len = &val_len.front();

  if (tdb_cons_add(cons_, uuid, timestamp, cvalues_entries, cval_len)) {
    throw TrailDBException();
  }
};

#endif  // CPP_TRAILDB_CONSTRUCTOR_H_INCLUDED
