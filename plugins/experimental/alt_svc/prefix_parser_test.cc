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

#include <string.h>
#include <iostream>
#include "prefix_parser.h"

/**
    Compare the output of a range with the resulting IP addresses.
    Provide, in this order, the prefix, the number on the "slash", the expected lower-bound, and the expected upper-bound.
*/
int ip_compare(const char *prefix, int prefix_num, const char *expected_lower, const char *expected_upper) {
    sockaddr_storage ip_lower, ip_upper;
    if (parse_addresses(prefix, prefix_num, &ip_lower, &ip_upper) != PrefixParseError::ok) {
        std::cout << "The parse should be okay in these cases." << std::endl;
        return 1;
    }
    size_t max = INET6_ADDRSTRLEN;
    char actual_lower[INET6_ADDRSTRLEN], actual_upper[INET6_ADDRSTRLEN];
    int fail = 0;
    if (strcmp(ats_ip_ntop((sockaddr*) &ip_lower, actual_lower, max), expected_lower) != 0) {
        std::cout << "Expected " << expected_lower << " for lower, but got: " << actual_lower << std::endl;
        fail = 1;
    }
    if (strcmp(ats_ip_ntop((sockaddr*) &ip_upper, actual_upper, max), expected_upper) != 0) {
        std::cout << "Expected " << expected_upper << " for upper, but got: " << actual_upper << std::endl;
        fail = 1;
    }
    return fail;
}

int expected_result(const char *prefix, int prefix_num, PrefixParseError expected) {
    sockaddr_storage ip_lower, ip_upper;
    PrefixParseError actual = parse_addresses(prefix, prefix_num, &ip_lower, &ip_upper);
    if (actual != expected) {
        std::cout << "Did not get expected error." << std::endl;
        return 1;
    }
    return 0;
}

int
main()
{
    // If all tests pass, return 0. If any one of these test fail, return 1.
  int fail =
      ip_compare("192.168.100.0", 22, "192.168.100.0", "192.168.103.255") // Test basic IPv4 prefix
    | ip_compare("127.0.0.1", 32, "127.0.0.1", "127.0.0.1") // Test IPv4 full prefix
    | ip_compare("127.0.0.1", 31, "127.0.0.0", "127.0.0.1") // Test IPv4 almost-full prefix
    | ip_compare("123.231.98.76", 0, "0.0.0.0", "255.255.255.255") // Test IPv4 none pizza w/ left beef

    | ip_compare("2001:db8::", 48, "2001:db8::", "2001:db8:0:ffff:ffff:ffff:ffff:ffff") // Test basic IPv6 prefix
    | ip_compare("1000::", 120, "1000::", "1000::ff") // Test IPv6 prefix on byte
    | ip_compare("1000::", 121, "1000::", "1000::7f") // Test IPv6 prefix cross-byte (under)
    | ip_compare("1000::", 119, "1000::", "1000::1ff") // Test IPv6 prefix cross-byte (over)
    | ip_compare("1000::", 111, "1000::", "1000::1:ffff") // Test another IPv6 prefix cross-byte (over)
    | ip_compare("::1", 128, "::1", "::1") // Test IPv6 Loopback prefix (full)
    | ip_compare("1234:5678::9abc:def0", 0, "::", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff") // Test IPv6 none pizza w/ left beef
    | ip_compare("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", 128, "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", "ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff") // Test a buffer stress test

    | expected_result("192.168.100.0", -1, PrefixParseError::bad_prefix) // Test negative prefix
    | expected_result("2001:db8::", 129, PrefixParseError::bad_prefix) // Test too-big prefix
    | expected_result("192.168.100.0", 33, PrefixParseError::bad_prefix) // Test too-big prefix IPv4

    | expected_result("lolwut", 4, PrefixParseError::bad_ip) // Test absolute nonsense IP address
    | expected_result("192.168.256.0", 4, PrefixParseError::bad_ip) // Test byte-madness IPv4 address
    | expected_result("123.68..0", 4, PrefixParseError::bad_ip) // Test missing byte IPv4 address
    | expected_result("1234::7a::ff", 48, PrefixParseError::bad_ip) // Test weird IPv6 address
    | expected_result("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff:1234", 128, PrefixParseError::bad_ip); // Test buffer busting

  return fail;
}
