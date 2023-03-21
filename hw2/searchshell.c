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

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600
#define BUF_SIZE 1024

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc
static void Usage(void);
static void ProcessQueries(DocTable* dt, MemIndex* mi);

static void PrintFileAndRank(LinkedList *ll, DocTable *dt);


//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char** argv) {
  if (argc != 2) {
    Usage();
  }

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.
  //
  // Note that you should make sure the fomatting of your
  // searchshell output exactly matches our solution binaries
  // to get full points on this part.

  char *dir_name = argv[1];
  DocTable *doc_table;
  MemIndex *mem_index;

  // crawl from the dir_name to produce doc_table and mem_index
  printf("Indexing '%s'\n", dir_name);
  bool crawl_success = CrawlFileTree(dir_name, &doc_table, &mem_index);
  if (!crawl_success) {
    fprintf(stderr, "Path: %s is not valid.\n", argv[1]);
    Usage();
  }

  ProcessQueries(doc_table, mem_index);

  // free resources within the same layer as allocation
  DocTable_Free(doc_table);
  MemIndex_Free(mem_index);

  return EXIT_SUCCESS;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}

static void ProcessQueries(DocTable* dt, MemIndex* mi) {
  // loop for asking user to input query
  while (true) {
    // prompt user
    printf("enter query:\n");
    char buffer[BUF_SIZE];

    if (fgets(buffer, BUF_SIZE, stdin) != NULL) {
      // check for '\n' input
      if (buffer[0] == '\n' || buffer[0] == ' ') continue;

      // first turn to lower case (dont change "\0")
      int i = 0;
      while (buffer[i] != '\0') {
        buffer[i] = (char)tolower((int)buffer[i]);
        i += 1;
      }

      // then split a query into words
      int words_num = 0;
      char *temp = buffer;
      char **query_split = (char **)malloc(BUF_SIZE * sizeof(char *));
      Verify333(query_split != NULL);  // don't forget malloc check
      while (true) {
        char *wd = strtok_r(temp, " ", &temp);
        if (wd == NULL || *wd == '\n') {
          // reach the query end
          break;
        }
        query_split[words_num] = wd;
        words_num += 1;
      }

      // set the last char to be null terminator
      char *c = strchr(query_split[words_num - 1], '\n');
      if (c) *c = '\0';

      // get linked list from MemIndex
      LinkedList *res = MemIndex_Search(mi, query_split, words_num);
      if (res == NULL) {
        free(query_split);
        continue;  // output nothing
      }

      // print linked list (filename + rank)
      PrintFileAndRank(res, dt);
      LinkedList_Free(res, &free);
      free(query_split);
    } else {
      // user signals end-of-file (fgets() gets ctrlD)
      printf("\nshutting down...\n");
      // resources will be freed outside the loop
      break;
    }
  }
}

static void PrintFileAndRank(LinkedList *ll, DocTable *dt) {
  LLIterator *iter = LLIterator_Allocate(ll);
  if (iter == NULL) {
    return;  // should do nothing
  }
  while (true) {
    SearchResult *sr;
    LLIterator_Get(iter, (LLPayload_t *)&sr);
    DocID_t doc_id = sr->doc_id;
    int rank = sr->rank;

    // find path name based on doc ID
    char *path = DocTable_GetDocName(dt, doc_id);
    Verify333(path != NULL);
    printf("  %s (%d)\n", path, rank);
    if (!LLIterator_Next(iter)) break;  // reach the end
  }
  LLIterator_Free(iter);  // don't forget free
}

