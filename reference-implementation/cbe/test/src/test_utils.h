#pragma once

#include <string>
#include <vector>

std::string as_string(const std::vector<uint8_t>& value);

std::string make_string_with_length(int length);

std::vector<uint8_t> generate_array_length_field(int64_t length);

std::vector<uint8_t> make_7f_bytes(int length);

std::vector<uint8_t> make_incrementing_bytes(int length, int start_value=0);
