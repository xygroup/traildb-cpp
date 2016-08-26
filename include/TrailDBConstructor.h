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

class StringPolicy {
public:
  using fields_t = std::vector<std::string>;
  
protected:
  auto makeValues(const fields_t& values) -> std::pair<std::vector<const char*>, std::vector<std::uint64_t>>;
};

auto StringPolicy::makeValues(const fields_t &values) -> std::pair<std::vector<const char*>, std::vector<std::uint64_t>> {
  std::vector<const char*> val_entries;
  std::vector<std::uint64_t> val_len;

  for (auto& s : values) {
    val_entries.push_back(&s.front());
    val_len.push_back(s.size());
  }
  return std::make_pair(val_entries, val_len);
}


template <typename DataPolicy = StringPolicy>
class TrailDBConstructor : DataPolicy {
 public:
  TrailDBConstructor(std::string path, std::vector<std::string>& fields);
  ~TrailDBConstructor();
  void Finalize();
  void Append(TrailDB tdb);
  void Add(std::string cookie, std::uint64_t timestamp,
           const typename DataPolicy::fields_t& values);

 private:
  tdb_cons* cons_;

  bool finalized_;
  std::string opath_;
  std::vector<std::string> ofields_;
  static const unsigned TDB_KEY_SIZE_BYTES = 16;
};

template <typename DataPolicy>
TrailDBConstructor<DataPolicy>::TrailDBConstructor(std::string path,
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

template <typename DataPolicy>
TrailDBConstructor<DataPolicy>::~TrailDBConstructor() { tdb_cons_close(cons_); };

template <typename DataPolicy>
void TrailDBConstructor<DataPolicy>::Finalize() {
  if (!finalized_) {
    if (tdb_cons_finalize(cons_)) {
      throw TrailDBException();
    }
    finalized_ = true;
  }
}

template <typename DataPolicy>
void TrailDBConstructor<DataPolicy>::Append(TrailDB tdb) {
  if (tdb_cons_append(cons_, tdb.GetDB())) {
    throw TrailDBException();
  }
}

template <typename DataPolicy>
void TrailDBConstructor<DataPolicy>::Add(const std::string hexuuid,
                             const std::uint64_t timestamp,
                             const typename DataPolicy::fields_t& values) {
  std::uint8_t uuid[16];
  const std::uint8_t* hex_str =
      reinterpret_cast<const std::uint8_t*>(hexuuid.c_str());
  tdb_uuid_raw(hex_str, uuid);

  auto val = DataPolicy::makeValues(values);
  const char** cvalues_entries = &(val.first.front());
  const std::uint64_t* cval_len = &(val.second.front());

  if (tdb_cons_add(cons_, uuid, timestamp, cvalues_entries, cval_len)) {
    throw TrailDBException();
  }
};

#endif  // CPP_TRAILDB_CONSTRUCTOR_H_INCLUDED
