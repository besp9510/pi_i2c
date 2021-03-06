# Inter-Integrated Circuit (I2C) Library for the Raspberry Pi
#
# Copyright (c) 2021 Benjamin Spencer
# =============================================================================
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
# OTHER DEALINGS IN THE SOFTWARE.
# =============================================================================

class ESETUPError(Exception):
    pass


class ENOPIVERError(Exception):
    pass


class ENACKError(Exception):
    pass


class EBADXFRError(Exception):
    pass


class EBADREGADDRError(Exception):
    pass


class ECLKTIMEOUTError(Exception):
    pass


class EI2CNOTCFGError(Exception):
    pass


class ENACKRSTError(Exception):
    pass


class EBUSLOCKUPError(Exception):
    pass


class EBUSUNKERRError(Exception):
    pass


class EFAILSTCONDError(Exception):
    pass


class EdeviceHUNGError(Exception):
    pass


class EINVALError(Exception):
    pass


class MAP_FAILEDError(Exception):
    pass

# Error numbers:
libpii2c_errno_list = [
    {"errno": "ENOPIVER", "value": 140, "raise": ENOPIVERError, "message": "Could not get PI board revision"},
    {"errno": "ENACK", "value": 141, "raise": ENACKError, "message": "Device did not acknowledge device address"},
    {"errno": "EBADXFR", "value": 142, "raise": EBADXFRError,
     "message": "Device did not acknowledge during byte transfer (read or write)"},
    {"errno": "EBADREGADDR", "value": 143, "raise": EBADREGADDRError,
     "message": "Device did not acknowledge register address"},
    {"errno": "ECLKTIMEOUT", "value": 144, "raise": ECLKTIMEOUTError,
     "message": "Device not responsive after clock stretch timeoutp"},
    {"errno": "EI2CNOTCFG", "value": 145, "raise": EI2CNOTCFGError, "message": "pi_i2c has not yet been configured"},
    {"errno": "ENACKRST", "value": 146, "raise": ENACKRSTError,
     "message": "Device did not respond after repeated start device address (read)"},
    {"errno": "EBUSLOCKUP", "value": 147, "raise": EBUSLOCKUPError,
     "message": "Bus is locked: SDA and SCL lines are being held low by device"},
    {"errno": "EBUSUNKERR", "value": 148, "raise": EBUSUNKERRError,
     "message": "Bus is in an unexpected state following an unknown error"},
    {"errno": "EFAILSTCOND", "value": 149, "raise": EFAILSTCONDError,
     "message": "Failed to write a START condition to the bus. Most likely cause is previously seen bus " +
     "error occurring during STOP condition."},
    {"errno": "EdeviceHUNG", "value": 150, "raise": EdeviceHUNGError, "message": "Device forcing SDA line low"},
    {"errno": "EINVAL", "value": 22, "raise": EINVALError, "message": "Invalid argument"},
    {"errno": "MAP_FAILED", "value": 1, "raise": MAP_FAILEDError,
     "message": "Memory map failed (most likely due to permissions)"}]
