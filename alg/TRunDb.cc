///////////////////////////////////////////////////////////////////////////////
// table: a FHICL table
// should have a "run_range : [r1, r2]" field
///////////////////////////////////////////////////////////////////////////////
#include <filesystem>
namespace fs = std::filesystem;

#include <iostream>
#include <string>
#include <format>
#include "Stntuple/alg/TRunDb.hh"
//
#include <iostream>
#include <string>
#include <vector>
#include <format>

#include "TEnv.h"

#include "cetlib/filepath_maker.h"

#include "TRACE/trace.h"

namespace stntuple {
//-----------------------------------------------------------------------------
TRunDb::TRunDb() {
  // initialize
}

//-----------------------------------------------------------------------------
TRunDb::~TRunDb() {
}

//-----------------------------------------------------------------------------
TRunDb* TRunDb::Instance() {
  static TRunDb* run_db(nullptr);

  if  (run_db == nullptr) {
    run_db     = new TRunDb();
  }
  return run_db;
}

//-----------------------------------------------------------------------------
int TRunDb::GetRunInfo(int RunNumber, TRunDb::Data_t* RunInfo) {
  int rc(0);

  RunInfo->run_number = -1;
  
  int subdir = (RunNumber / 1000 )*1000;
  
  std::string fn = std::format("{}/daqana/rundb/{:06d}/{:06d}_runinfo.toml",getenv("SPACK_ENV"),subdir,subdir);

  if (not fs::exists(fn)) {
    TLOG(TLVL_ERROR) << std::format("file {} doesn't exist. Forgot to create?",fn);
    return -1;
  }

  /* TODO : switch to fcl

  toml::table tbl = toml::parse_file(fn);
    
  auto maps = tbl["runinfo"].as_array();
  //  int n_run_ranges = maps->size();

  for (auto&& node : *maps) {
    toml::table& tbl = *node.as_table();

    int rn = tbl["run_number"].value_or(-1);
    if (rn == RunNumber) {
      RunInfo->run_number   = RunNumber;                            // for kicks
      RunInfo->run_type     = tbl["run_type"     ].value_or(-1);
      RunInfo->event_mode   = tbl["event_mode"   ].value_or(1); // by default, on-spill 
      RunInfo->cfo_rate     = tbl["cfo_rate"     ].value_or(0.0); // to mark undefined
      RunInfo->ew_length    = tbl["ew_length"    ].value_or(0.0); // to mark undefined
      RunInfo->trigger_rate = tbl["trigger_rate" ].value_or(0.0); // to mark undefined
      RunInfo->max_ewtag    = tbl["max_ewtag"    ].value_or(0.0); // to mark undefined

      if (RunInfo->run_type == e_PULSE_INJECTION) {
        // for pulse injection runs need to know pulsed channels, normally, one out of eight
        auto arr = tbl["pulsed_channels"].as_array();
        for (int i=0; i<12; i++) {
          auto v = arr->get_as<int64_t>(i);
          RunInfo->pulsed_channel[i] = v->value_or(-1);
        }
      }
      
      break;
    }
  }
  */
  RunInfo->run_type    = 1;                // for kicks
  return rc;
}

//-----------------------------------------------------------------------------
  fhicl::ParameterSet TRunDb::GetTable(const char* TableName, int RunNumber) {

    cet::filepath_lookup policy("FHICL_FILE_PATH");

    std::string rundb_dir = gEnv->GetValue("Stntuple.RunDb","");
    std::string fn        = std::format("{}/{}/{}.fcl",rundb_dir,TableName,TableName);
    auto const pset       = fhicl::ParameterSet::make(fn, policy);

    fhicl::ParameterSet p;
    // retrieved is a vector of tables corresponding to different run ranges
  
    auto list_of_tables = pset.get<std::vector<fhicl::ParameterSet>>(TableName);

    for (size_t i = 0; i < list_of_tables.size(); ++i) {
      auto const& entry = list_of_tables[i];
      auto run_range    = entry.get<std::vector<int>>("run_range");

      if ((RunNumber >= run_range[0]) and (RunNumber <= run_range[1])) {
        // found the needed run range, return the table

        p  = entry;
        break;
      }
    }
    return p;
  }
}

