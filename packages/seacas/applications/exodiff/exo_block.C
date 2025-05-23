// Copyright(C) 1999-2023 National Technology & Engineering Solutions
// of Sandia, LLC (NTESS).  Under the terms of Contract DE-NA0003525 with
// NTESS, the U.S. Government retains certain rights in this software.
//
// See packages/seacas/LICENSE for details

#include "ED_SystemInterface.h" // for SystemInterface, interFace
#include "exo_block.h"
#include "exodusII.h" // for ex_block, etc
#include "fmt/ostream.h"
#include "smart_assert.h" // for SMART_ASSERT
#include <cstdlib>        // for exit, nullptr
#include <string>         // for string, char_traits

template <typename INT> Exo_Block<INT>::Exo_Block() : Exo_Entity() {}

template <typename INT>
Exo_Block<INT>::Exo_Block(int file_id, size_t exo_block_id) : Exo_Entity(file_id, exo_block_id)
{
  SMART_ASSERT(file_id >= 0);
  SMART_ASSERT((int)exo_block_id > EX_INVALID_ID);

  initialize(file_id, exo_block_id);
}

template <typename INT>
Exo_Block<INT>::Exo_Block(int file_id, size_t id, const char *type, size_t num_e, size_t num_npe)
    : Exo_Entity(file_id, id, num_e), elmt_type(type), num_nodes_per_elmt(num_npe)
{
  SMART_ASSERT(id > 0);
  SMART_ASSERT(!elmt_type.empty());
  SMART_ASSERT(num_npe > 0);
}

template <typename INT> EXOTYPE Exo_Block<INT>::exodus_type() const { return EX_ELEM_BLOCK; }

template <typename INT> void Exo_Block<INT>::entity_load_params()
{
  int      num_attr;
  ex_block block{};
  block.id   = id_;
  block.type = EX_ELEM_BLOCK;
  int err    = ex_get_block_param(fileId, &block);

  if (err < 0) {
    Error("Exo_Block<INT>::entity_load_params(): Failed to get element"
          " block parameters!  Aborting...\n");
  }

  numEntity          = block.num_entry;
  num_nodes_per_elmt = block.num_nodes_per_entry;
  num_attr           = block.num_attribute;
  elmt_type          = block.topology;

  if (num_nodes_per_elmt < 0 || num_attr < 0) {
    Error(fmt::format(
        fmt::runtime("Exo_Block<INT>::entity_load_params(): Data appears corrupt for block {}!\n"
                     "\tnum elmts          = {}\n"
                     "\tnum nodes per elmt = {}\n"
                     "\tnum attributes     = {}\n"
                     " ... Aborting...\n"),
        fmt::group_digits(numEntity), num_nodes_per_elmt, num_attr));
  }
}

template <typename INT> std::string Exo_Block<INT>::Load_Connectivity()
{
  SMART_ASSERT(Check_State());

  if (fileId < 0) {
    return "ERROR:  Invalid file id!";
  }
  if (id_ == EX_INVALID_ID) {
    return "ERROR:  Must initialize block parameters first!";
  }
  if (!conn.empty()) {
    conn.clear();
  }

  if (numEntity && num_nodes_per_elmt) {
    conn.resize(numEntity * num_nodes_per_elmt);

    int err = ex_get_conn(fileId, EX_ELEM_BLOCK, id_, Data(conn), nullptr, nullptr);
    if (err < 0) {
      Error(fmt::format("Exo_Block<INT>::Load_Connectivity(): Call to ex_get_conn returned error "
                        "value!  Block id = {}\n"
                        "Aborting...\n",
                        id_));
    }
    else if (err > 0) {
      return fmt::format("WARNING:  Number {} returned from call to ex_get_conn()", err);
    }
  }

  return "";
}

template <typename INT> std::string Exo_Block<INT>::Free_Connectivity()
{
  conn.clear();
  SMART_ASSERT(conn.empty());
  return "";
}

template <typename INT> const INT *Exo_Block<INT>::Connectivity(size_t elmt_index) const
{
  SMART_ASSERT(Check_State());

  if (conn.empty() || elmt_index >= numEntity) {
    return nullptr;
  }

  return &conn[elmt_index * num_nodes_per_elmt];
}

template <typename INT> int Exo_Block<INT>::Check_State() const
{
  SMART_ASSERT(id_ >= EX_INVALID_ID);
  SMART_ASSERT(!(id_ == EX_INVALID_ID && !elmt_type.empty()));
  SMART_ASSERT(!(id_ == EX_INVALID_ID && num_nodes_per_elmt >= 0));
  SMART_ASSERT(!(id_ == EX_INVALID_ID && !conn.empty()));

  SMART_ASSERT(!(!conn.empty() && (numEntity == 0 || num_nodes_per_elmt <= 0)));

  return 1;
}
template class Exo_Block<int>;
template class Exo_Block<int64_t>;
