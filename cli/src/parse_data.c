// Inter-Integrated Circuit (I2C) Library for the Raspberry Pi
//
// ... but this time in the CLI
//
// Copyright (c) 2022 Benjamin Spencer
// ============================================================================
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
// OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.
// ============================================================================

// Include C standard libraries:
#include <stdlib.h> // C Standard library
#include <string.h> // C Standard string manipulation libary

// Include C POSIX libraries:
#include <ctype.h>  // C compatible data types

// Parse input string list of hex numbers into an integer array
int parse_data(char *data_string, int *data_parsed) {
    char* token_string;
    char* token_hex;
    char *base;

    char* data_parsed_string = NULL;

    int i = 0;
    unsigned int parse_counter;
    unsigned int data_parsed_hex;

    char* rest = data_string;

    // This splits the string by the comma delimeter. Nested loop splits
    // hex string into string numbers for later conversion.
    while ((token_string = strtok_r(rest, ",", &rest))) {
        // Reset so we can catch any improperly formatted hex strings:
        parse_counter = 0;

        // Check if input data is hex with a lower or upper case:
        if (strstr(token_string, "0x") || strstr(token_string, "0X")) {
            while ((token_hex = strtok_r(token_string, "x", &token_string))) {
                // Error out if we have looped too many times. This means that
                // the hex was improperly formatted somehow (e.g., 0x01x32)
                parse_counter++;
                if (parse_counter > 2) {
                    return 0;
                }

                // Save off parsed data for later conversion:
                data_parsed_string = token_hex;
            }
        // This means that we did not get properly formmated hex values:
        } else {
            return 0;
        }

        // Convert hex string into an integer & save:
        data_parsed_hex = strtol(data_parsed_string, &base, 16);
        data_parsed[i] = data_parsed_hex;

        // Keep track of how many bytes have been parsed so far:
        i++;
    }

    // Returning the number of bytes parsed:
    return i;
}