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


class ESLAVEHUNGError(Exception):
    pass


class EINVALError(Exception):
    pass


class MAP_FAILEDError(Exception):
    pass

# Error numbers:
libpii2c_errno_list = [
    {"errno": "ENOPIVER", "value": 140, "raise": ENOPIVERError, "message": "Could not get PI board revision"},
    {"errno": "ENACK", "value": 141, "raise": ENACKError, "message": "Slave did not acknowledge slave address"},
    {"errno": "EBADXFR", "value": 142, "raise": EBADXFRError,
     "message": "Slave did not acknowledge during byte transfer (read or write)"},
    {"errno": "EBADREGADDR", "value": 143, "raise": EBADREGADDRError,
     "message": "Slave did not acknowledge register address"},
    {"errno": "ECLKTIMEOUT", "value": 144, "raise": ECLKTIMEOUTError,
     "message": "Slave not responsive after clock stretch timeoutp"},
    {"errno": "EI2CNOTCFG", "value": 145, "raise": EI2CNOTCFGError, "message": "pi_i2c has not yet been configured"},
    {"errno": "ENACKRST", "value": 146, "raise": ENACKRSTError,
     "message": "Slave did not respond after repeated start slave address (read)"},
    {"errno": "EBUSLOCKUP", "value": 147, "raise": EBUSLOCKUPError,
     "message": "Bus is locked: SDA and SCL lines are being held low by slave"},
    {"errno": "EBUSUNKERR", "value": 148, "raise": EBUSUNKERRError,
     "message": "Bus is in an unexpected state following an unknown error"},
    {"errno": "EFAILSTCOND", "value": 149, "raise": EFAILSTCONDError,
     "message": "Failed to write a START condition to the bus. Most likely cause is previously seen bus " +
     "error occurring during STOP condition."},
    {"errno": "ESLAVEHUNG", "value": 150, "raise": ESLAVEHUNGError, "message": "Slave forcing SDA line low"},
    {"errno": "EINVAL", "value": 22, "raise": EINVALError, "message": "Invalid argument"},
    {"errno": "MAP_FAILED", "value": 1, "raise": MAP_FAILEDError,
     "message": "Memory map failed (most likely due to permissions)"}]
