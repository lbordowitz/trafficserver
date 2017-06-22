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
#include <fstream>

std::string SingleServiceFileMap::findHostForIP(IpEndpoint * ip, std::string hostname) const noexcept {
    return this->findHostForIP(ip);
}

std::string SingleServiceFileMap::findHostForIP(IpEndpoint * ip) const noexcept {
    return this->file_contents;
}

SingleServiceFileMap::SingleServiceFileMap(std::string filename) {
    // Read file
    std::ifstream config_file {filename};
    if (!config_file) {
        std::cout << "Cannot find config file at " << filename << std::endl;
        // TODO how to fail in init?
    }
    // Parse file into plugin-local IpMap
    config_file >> this->file_contents;
    // Fail with a "nice message"
}