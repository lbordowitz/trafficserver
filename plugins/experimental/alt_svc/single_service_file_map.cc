/**
  @file
  @brief TODO documentation

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
#include <iostream>
#include <algorithm>

using namespace std;

string SingleServiceFileMap::findHostForIP(IpEndpoint * ip, string hostname) const noexcept {
    return this->findHostForIP(ip);
}

string SingleServiceFileMap::findHostForIP(IpEndpoint * ip) const noexcept {
    char * output;
    return this->host_map.contains(ip, (void**) &output) ? output : "";
}

SingleServiceFileMap::SingleServiceFileMap(string filename) {
    // Read file
    int fail = 0;
    ifstream config_file {filename};
    if (config_file.fail()) {
        cout << "Cannot find config file at " << filename << endl;
        fail = 1;
        // TODO how to fail in init?
    } else {
        // Parse file into plugin-local IpMap
        string hostname, ip_with_prefix, buff;
        while (!getline(config_file, buff).eof()) {
            if (buff[0] == ' ') {
                ip_with_prefix = buff.erase(remove_if(buff.begin(), buff.end(), isspace), buff.end());
                size_t slash;
                slash = ip_with_prefix.find('/');
                if (slash == string::npos) {
                    cout << "can't find a slash, bro" << endl;
                    fail = 1;
                    // TODO how to fail in init?
                } else if (hostname.empty()) {
                    cout << "hostname aint there, hoss" << endl;
                    fail = 1;
                } else {
                    string ip = ip_with_prefix.substr(0, slash);
                    int prefix_num = stoi(ip_with_prefix.substr(slash + 1));
                    sockaddr_storage lower, upper;
                    if (parse_addresses(ip.c_str(), prefix_num, &lower, &upper) == PrefixParseError::ok) {
                        // We should be okay adding this to the map!
                        this->host_map.mark((sockaddr *) &lower, (sockaddr *) &upper, const_cast<char*>(hostname.c_str()));
                    } else {
                        // Error message should already be set here, just make fail be 1.
                        fail = 1;
                    }
                }
            } else {
                hostname = buff.erase(remove_if(buff.begin(), buff.end(), isspace), buff.end());
            }
        }
    }
    // TODO Fail with a "nice message"
    if (fail) {
        cout << "init failed bye" << endl;
    }
    // TODO there's a lot of nesting going on, is there a better way?
}