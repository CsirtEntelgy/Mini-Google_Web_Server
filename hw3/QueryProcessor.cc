/*
 * Copyright ©2023 Justin Hsia.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2023 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>
#include <list>
#include <string>
#include <vector>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

static list<DocIDElementHeader> MergeDocIDElementHeaderList(
  const list<DocIDElementHeader>& list1,
  const list<DocIDElementHeader>& list2,
  bool first_flag);

QueryProcessor::QueryProcessor(const list<string>& index_list, bool validate) {
  // Stash away a copy of the index list.
  index_list_ = index_list;
  array_len_ = index_list_.size();
  Verify333(array_len_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader* [array_len_];
  itr_array_ = new IndexTableReader* [array_len_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = index_list_.begin();
  for (int i = 0; i < array_len_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < array_len_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t doc_id;  // The document ID within the index file.
  int     rank;    // The rank of the result so far.
} IdxQueryResult;

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string>& query) const {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> final_result;

  // Traverse thr index lists
  for (int i = 0; i < array_len_; i++) {
    // Initialize readers.
    DocTableReader* dtr = dtr_array_[i];
    IndexTableReader* itr = itr_array_[i];
    // Define a list of DocIDElementHeader to store results.
    list<DocIDElementHeader> did_result;
    // For each word in the query.
    for (uint j = 0; j < query.size(); j++) {
      DocIDTableReader* didtr = itr->LookupWord(query[j]);
      // If no match for current word, clean the result and break.
      if (didtr == nullptr) {
        did_result.clear();
        delete didtr;
        break;
      }
      list<DocIDElementHeader> result_j = didtr->GetDocIDList();
      // Merge the result of current word to the overall result.
      did_result = MergeDocIDElementHeaderList(did_result, result_j, j==0);
      // Check if the overall result list is already empty.
      if (did_result.size() == 0) {
        delete didtr;
        break;
      }
      delete didtr;
    }

    // Use Iterator to fill final_result.
    list<DocIDElementHeader>::iterator it;
    for (auto const& did : did_result) {
      QueryResult qr;
      Verify333(dtr->LookupDocID(did.doc_id, &qr.document_name));
      qr.rank = did.num_positions;
      final_result.push_back(qr);
    }
  }

  // Sort the final results.
  sort(final_result.begin(), final_result.end());
  return final_result;
}

// A helper function to merge two list<DocIDElementHeader>.
static list<DocIDElementHeader> MergeDocIDElementHeaderList(
  const list<DocIDElementHeader>& list1,
  const list<DocIDElementHeader>& list2,
  bool first_flag) {
  // If it's the first merge, which indicates list1 is empty, return list2.
  if (first_flag == true) {
    return list2;
  }
  list<DocIDElementHeader> merged_list;
  // Use a double loop to do the merge.
  for (auto const& i : list1) {
    for (auto const& j : list2) {
      if (i.doc_id == j.doc_id) {
        merged_list.push_back(DocIDElementHeader(
          i.doc_id, i.num_positions + j.num_positions));
        break;
      }
    }
  }
  return merged_list;
}

}  // namespace hw3
