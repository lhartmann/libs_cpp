#ifndef MODBUS_ERRORS_BY_NAME_H
#include "modbus_errors.h"

/// Gets a user-friendly description for a Modbus error code.
inline const char *modbus_error_by_name(int err) {
	switch (err) {
	case MBE_ILLEGAL_FUNCTION: return "Function not implemented on device.";
	case MBE_ILLEGAL_DATA_ADDRESS: return "Address range not implemented on device.";
	case MBE_ILLEGAL_DATA_VALUE: return "Request fields contained bad data (should not ever happen).";
	case MBE_TOO_MANY: return "Desired register count too large.";
	case MBE_ADDRESS_OVERFLOW: return "Desired read range overflows the address range.";
	case MBE_BAD_RESPONSE_LENGTH: return "Received response length differs from expected.";
	case MBE_NOT_CONNECTED: return "Communication link to the device was not established properly.";
	case MBE_NO_MEMORY: return "Out of memory (unlikely to ever happen).";
	case MBE_SEND_REQUEST_FAILED: return "Internal error sending request (write syscall failed).";
	case MBE_BAD_POINTERS: return "Either input buffer data or length pointers were NULL.";
	case MBE_RECEIVE_FAILED: return "Internal error reading response (select or read syscalls failed).";
	case MBE_UNEXPECTED_DEVICE_RESPONDED: return "Response received from a device different of the request.";
	case MBE_RESPONSE_TOO_LONG: return "Response was larger than the available buffer.";
	case MBE_TIMED_OUT: return "No response received on time.";
	case MBE_CRC_FAIL: return "Response CRC16 check did not match.";
	case MBE_BAD_TRANSACTION_ID: return "Transaction ID does not match request (MODBUS-TCP)";
	case MBE_BAD_PROTOCOL_ID: return "Protocol ID is not zero (MODBUS-TCP)";
	}
	return "Unknown error.";
}
#endif
