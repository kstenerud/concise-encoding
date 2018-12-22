#pragma once

#include <string>
#include <vector>

std::string as_string(const std::vector<uint8_t>& value);

// Generate an array length field (length << 2, sized appropriately)
std::vector<uint8_t> generate_array_length_field(int64_t length);

// Generate a string with the sequence 0,1,2,3,4,5,6,7,8,9,0,1...
std::string make_string(int length);

// Generate a vector with the sequence 7f,7f,7f...
std::vector<uint8_t> make_7f_bytes(int length);

// Generate a vector with the sequence 00,01,02...7f,00,01...
std::vector<uint8_t> make_incrementing_bytes(int length, int start_value=0);
