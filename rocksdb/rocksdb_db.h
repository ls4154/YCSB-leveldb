//
//  rocksdb_db.h
//  YCSB-cpp
//
//  Copyright (c) 2020 Youngjae Lee <ls4154.lee@gmail.com>.
//

#ifndef YCSB_C_ROCKSDB_DB_H_
#define YCSB_C_ROCKSDB_DB_H_

#include <iostream>
#include <string>
#include <mutex>

#include "core/db.h"
#include "core/properties.h"

#include <rocksdb/db.h>
#include <rocksdb/options.h>

namespace ycsbc {

// TODO table handling (ycsb core use only single table)
class RocksdbDB : public DB {
 public:
  RocksdbDB() : db_(nullptr), init_done_(false) {}

  ~RocksdbDB();

  void Init();

  void Cleanup();

  Status Read(const std::string &table, const std::string &key,
              const std::vector<std::string> *fields, std::vector<Field> &result) {
    return (this->*(method_read_))(table, key, fields, result);
  }

  Status Scan(const std::string &table, const std::string &key, int len,
              const std::vector<std::string> *fields, std::vector<std::vector<Field>> &result) {
    return (this->*(method_scan_))(table, key, len, fields, result);
  }

  Status Update(const std::string &table, const std::string &key, std::vector<Field> &values) {
    return (this->*(method_update_))(table, key, values);
  }

  Status Insert(const std::string &table, const std::string &key, std::vector<Field> &values) {
    return (this->*(method_insert_))(table, key, values);
  }

  Status Delete(const std::string &table, const std::string &key) {
    return (this->*(method_delete_))(table, key);
  }

 private:
  enum RocksFormat {
    kSingleEntry,
    kRowMajor,
    kColumnMajor
  };
  RocksFormat format_;
  void GetOptions(const utils::Properties &props, rocksdb::Options *opt);
  void SerializeRow(const std::vector<Field> &values, std::string *data);
  void DeserializeRowFilter(std::vector<Field> *values, const std::string &data,
                            const std::vector<std::string> &fields);
  void DeserializeRow(std::vector<Field> *values, const std::string &data);
  std::string BuildCompKey(const std::string &key, const std::string &field_name);
  std::string KeyFromCompKey(const std::string &comp_key);
  std::string FieldFromCompKey(const std::string &comp_key);

  Status ReadSingleEntry(const std::string &table, const std::string &key,
                         const std::vector<std::string> *fields, std::vector<Field> &result);
  Status ScanSingleEntry(const std::string &table, const std::string &key, int len,
                         const std::vector<std::string> *fields,
                         std::vector<std::vector<Field>> &result);
  Status UpdateSingleEntry(const std::string &table, const std::string &key,
                           std::vector<Field> &values);
  Status InsertSingleEntry(const std::string &table, const std::string &key,
                           std::vector<Field> &values);
  Status DeleteSingleEntry(const std::string &table, const std::string &key);

  Status ReadCompKeyRM(const std::string &table, const std::string &key,
                       const std::vector<std::string> *fields, std::vector<Field> &result);
  Status ScanCompKeyRM(const std::string &table, const std::string &key, int len,
                       const std::vector<std::string> *fields,
                       std::vector<std::vector<Field>> &result);
  Status ReadCompKeyCM(const std::string &table, const std::string &key,
                       const std::vector<std::string> *fields, std::vector<Field> &result);
  Status ScanCompKeyCM(const std::string &table, const std::string &key, int len,
                       const std::vector<std::string> *fields,
                       std::vector<std::vector<Field>> &result);
  Status InsertCompKey(const std::string &table, const std::string &key,
                       std::vector<Field> &values);
  Status DeleteCompKey(const std::string &table, const std::string &key);

  rocksdb::DB *db_;
  Status (RocksdbDB::*method_read_)(const std::string &, const std:: string &,
                                    const std::vector<std::string> *, std::vector<Field> &);
  Status (RocksdbDB::*method_scan_)(const std::string &, const std::string &,
                                    int, const std::vector<std::string> *,
                                    std::vector<std::vector<Field>> &);
  Status (RocksdbDB::*method_update_)(const std::string &, const std::string &,
                                      std::vector<Field> &);
  Status (RocksdbDB::*method_insert_)(const std::string &, const std::string &,
                                      std::vector<Field> &);
  Status (RocksdbDB::*method_delete_)(const std::string &, const std::string &);
  int fieldcount_;

  bool init_done_;
  std::mutex mu_;
};

DB *NewRocksdbDB();

} // ycsbc

#endif // YCSB_C_ROCKSDB_DB_H_
