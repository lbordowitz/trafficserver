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
#include <string>
#include <list>
#include <iostream>

using namespace std;

int
test_single_service_file_map(string file_location, list<tuple<IpEndpoint *, string> > in, list<IpEndpoint *> out)
{
  SingleServiceFileMap hostMap(location);
  int fail = 0;
  for (const auto pair : in) {
    string expected = get<1>(pair);
    char * actual = hostMap.findHostForIP(get<0>(pair));
    if(expected.compare(actual) != 0) {
      cout << "Expected host " << expected << ", but got " << actual << endl;
      fail = 1;
    }
  }

  for (const auto ip : out) {
    if (hostMap.findHostForIP(ip) != nullptr) {
      cout << "Found an IP address that wasn't expected in the file." << endl;
      fail = 1;
    }
  }
  return fail;
}

int
main()
{
  // Shamelessly copied from the IpMapTest
  IpEndpoint a0, a_10_28_56_0, a_10_28_56_255, a3, a4;
  IpEndpoint a_9_255_255_255, a_10_0_0_0, a_10_0_0_19, a_10_0_0_255, a_10_0_1_0;
  IpEndpoint a_10_28_56_4, a_max, a_loopback, a_loopback2;
  IpEndpoint a_10_28_55_255, a_10_28_57_0;
  IpEndpoint a_63_128_1_12;
  IpEndpoint a_0000_0000, a_0000_0001, a_ffff_ffff;
  IpEndpoint a_fe80_9d8f, a_fe80_9d90, a_fe80_9d95, a_fe80_9d9d, a_fe80_9d9e;

  ats_ip_pton("0.0.0.0", &a0);
  ats_ip_pton("255.255.255.255", &a_max);

  ats_ip_pton("9.255.255.255", &a_9_255_255_255);
  ats_ip_pton("10.0.0.0", &a_10_0_0_0);
  ats_ip_pton("10.0.0.19", &a_10_0_0_19);
  ats_ip_pton("10.0.0.255", &a_10_0_0_255);
  ats_ip_pton("10.0.1.0", &a_10_0_1_0);

  ats_ip_pton("10.28.55.255", &a_10_28_55_255);
  ats_ip_pton("10.28.56.0", &a_10_28_56_0);
  ats_ip_pton("10.28.56.4", &a_10_28_56_4);
  ats_ip_pton("10.28.56.255", &a_10_28_56_255);
  ats_ip_pton("10.28.57.0", &a_10_28_57_0);

  ats_ip_pton("192.168.1.0", &a3);
  ats_ip_pton("192.168.1.255", &a4);

  ats_ip_pton("::", &a_0000_0000);
  ats_ip_pton("::1", &a_0000_0001);
  ats_ip_pton("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", &a_ffff_ffff);
  ats_ip_pton("fe80::221:9bff:fe10:9d8f", &a_fe80_9d8f);
  ats_ip_pton("fe80::221:9bff:fe10:9d90", &a_fe80_9d90);
  ats_ip_pton("fe80::221:9bff:fe10:9d95", &a_fe80_9d95);
  ats_ip_pton("fe80::221:9bff:fe10:9d9d", &a_fe80_9d9d);
  ats_ip_pton("fe80::221:9bff:fe10:9d9e", &a_fe80_9d9e);

  ats_ip_pton("127.0.0.0", &a_loopback);
  ats_ip_pton("127.0.0.255", &a_loopback2);
  ats_ip_pton("63.128.1.12", &a_63_128_1_12);
  // End shameless copy from IpMapTest

  string test1_location = "example_configs/single_service_file/test1.txt";
  list< tuple<IpEndpoint *, string> > in (1, make_tuple(&a_63_128_1_12, "nebraska.example.com"));
  list<IpEndpoint *> out (1, &a_10_28_56_4);

  // First test: simple file with one prefix definition
  int fail = test_single_service_file_map(test1_location, in, out);

  return fail;
}
