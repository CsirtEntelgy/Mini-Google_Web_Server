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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char* kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;
static const int kReadLen = 1024;

bool HttpConnection::GetNextRequest(HttpRequest* const request) {
  // Use WrappedRead from HttpUtils.cc to read bytes from the files into
  // private buffer_ variable. Keep reading until:
  // 1. The connection drops
  // 2. You see a "\r\n\r\n" indicating the end of the request header.
  //
  // Hint: Try and read in a large amount of bytes each time you call
  // WrappedRead.
  //
  // After reading complete request header, use ParseRequest() to parse into
  // an HttpRequest and save to the output parameter request.
  //
  // Important note: Clients may send back-to-back requests on the same socket.
  // This means WrappedRead may also end up reading more than one request.
  // Make sure to save anything you read after "\r\n\r\n" in buffer_ for the
  // next time the caller invokes GetNextRequest()!

  // STEP 1:
  size_t pos = buffer_.find(kHeaderEnd);
  if (pos == string::npos) {
    int byte_read;
    unsigned char buf[kReadLen];
    while (true) {
      byte_read = WrappedRead(fd_, buf, kReadLen);
      if (byte_read == 0) {
        // EOF or connection dropped happens
        break;
      } else if (byte_read == -1) {
        // read failed because of fatal error
        return false;
      } else {
        // append the bytes to buffer_
        buffer_ += string(reinterpret_cast<char*>(buf), byte_read);

        // stop reading if we've read "\r\n\r\n"
        pos = buffer_.find(kHeaderEnd);
        if (pos != string::npos)
          break;
      }
    }
  }

  // check whether the request header ends with "\r\n\r\n"
  if (pos == std::string::npos)
    return false;

  // parse the header and store it into output parameter
  *request = ParseRequest(buffer_.substr(0, pos + kHeaderEndLen));

  // remove the proportion of finished parsing
  buffer_ = buffer_.substr(pos + kHeaderEndLen);

  return true;
}

bool HttpConnection::WriteResponse(const HttpResponse& response) const {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         reinterpret_cast<const unsigned char*>(str.c_str()),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string& request) const {
  HttpRequest req("/");  // by default, get "/".

  // Plan for STEP 2:
  // 1. Split the request into different lines (split on "\r\n").
  // 2. Extract the URI from the first line and store it in req.URI.
  // 3. For the rest of the lines in the request, track the header name and
  //    value and store them in req.headers_ (e.g. HttpRequest::AddHeader).
  //
  // Hint: Take a look at HttpRequest.h for details about the HTTP header
  // format that you need to parse.
  //
  // You'll probably want to look up boost functions for:
  // - Splitting a string into lines on a "\r\n" delimiter
  // - Trimming whitespace from the end of a string
  // - Converting a string to lowercase.
  //
  // Note: If a header is malformed, skip that line.

  // STEP 2:
  // Get the request
  string rq = request;
  // Split the request into lines with deliminator "\r\n"
  vector<string> lines;
  boost::algorithm::split(lines, rq, boost::is_any_of("\r\n"),
                        boost::algorithm::token_compress_on);

  // Split the first line.
  std::vector<std::string> line;
  boost::split(line, lines[0], boost::is_any_of(" "),
               boost::token_compress_on);
  req.set_uri(line[1]);

  // Get header name and value for each line except the first
  for (uint32_t i = 1; i < lines.size() - 1; i += 1) {
    // Split the line
    boost::split(line, lines[i], boost::is_any_of(":"),
                 boost::token_compress_on);
    // Turn header name to lower case and trim whitespace
    boost::to_lower(line[0]);
    boost::trim(line[1]);
    // Add header
    req.AddHeader(line[0], line[1]);
  }
  return req;
}

}  // namespace hw4
