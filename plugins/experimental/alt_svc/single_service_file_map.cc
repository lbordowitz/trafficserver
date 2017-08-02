/**
  @file
  @brief A particular IPHostMap implementation which takes a static file and routes client IPs based on that file.

  @section license License

  Licensed to the Apache Software Foundation (ASF) under one
  or more contributor license agreements.  See the NOTICE file
  distributed with this work for additional information
  regarding copyright ownership.  The ASF licenses this file
  to you under the Apache License, Version 2.0 (the
  "License"); you may not use this file except in compliance
  with the License.  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "ip_host_map.h"
#include "prefix_parser.h"
#include <fstream>
#include <algorithm>

using namespace std;

static char const *ssfm_empty = "";

string const &
SingleServiceFileMap::findHostForIP(const sockaddr *ip, string const &hostname) const noexcept
{
  return this->findHostForIP(ip);
}

string const &
SingleServiceFileMap::findHostForIP(const sockaddr *ip) const noexcept
{
  char *data           = nullptr;
  string const *output = new string(this->host_map.contains(ip, (void **)&data) ? data : ssfm_empty);

  return *output;
}

bool
SingleServiceFileMap::isValid() const noexcept
{
  return _isValid;
}

// Lifted from ControlMatcher.cc
void
SingleServiceFileMap::print_the_map() const noexcept
{
  TS_DEBUG(PLUGIN_NAME, "\tIp Matcher with %zu ranges.\n", this->host_map.getCount());
  for (IpMap::iterator spot(this->host_map.begin()), limit(this->host_map.end()); spot != limit; ++spot) {
    char b1[INET6_ADDRSTRLEN], b2[INET6_ADDRSTRLEN];
    TS_DEBUG(PLUGIN_NAME, "\tRange %s - %s ", ats_ip_ntop(spot->min(), b1, sizeof b1), ats_ip_ntop(spot->max(), b2, sizeof b2));
    TS_DEBUG(PLUGIN_NAME, "Host: %s \n", static_cast<char *>(spot->data()));
  }
}

SingleServiceFileMap::SingleServiceFileMap(string const &filename)
{
  // Read file
  bool fail = 0;
  ifstream config_file{filename};
  if (config_file.fail()) {
    TS_DEBUG(PLUGIN_NAME, "Cannot find a config file at: %s", filename.c_str());
    fail = 1;
    // TODO how to fail in init?
  } else {
    // Parse file into plugin-local IpMap
    string ip_with_prefix, buff;
    char *hostname = nullptr; // TODO can we make this a string?
    while (!getline(config_file, buff).eof()) {
      bool is_host = (buff[0] != ' ');
      buff.erase(remove_if(buff.begin(), buff.end(), ::isspace), buff.end());
      if (is_host) {
        hostname = static_cast<char *>(malloc(buff.size() + 1));
        strcpy(hostname, buff.data());
        continue;
      }
      ip_with_prefix = buff;

      size_t slash;
      slash = ip_with_prefix.find('/');
      if (slash == string::npos) {
        TSError("Cannot find a slash in the provided configuration prefix: %s", ip_with_prefix.c_str());
        fail = 1;
        continue;
      } else if (hostname == nullptr) {
        TSError("Did not find a hostname before the provided configuration prefix: %s", ip_with_prefix.c_str());
        fail = 1;
        continue;
      }

      string ip               = ip_with_prefix.substr(0, slash);
      int prefix_num          = stoi(ip_with_prefix.substr(slash + 1));
      sockaddr_storage *lower = new sockaddr_storage(), *upper = new sockaddr_storage();
      if (parse_addresses(ip.c_str(), prefix_num, lower, upper) == PrefixParseError::ok) {
        // We should be okay adding this to the map!
        TS_DEBUG(PLUGIN_NAME, "Mapping %s to host %s", ip_with_prefix.c_str(), hostname);
        this->host_map.mark((sockaddr *)lower, (sockaddr *)upper, static_cast<void *>(hostname));
      } else {
        // Error message should already be logged by now, just make fail be 1.
        fail = 1;
        continue;
      }
    }
  }
  // TODO Fail with a "nice message"
  if (fail) {
    TSError("Alt-Svc plugin initialization failed, this plugin is disabled");
  }

  // Let the plugin know that its configuration is invalid.
  _isValid = fail;
}