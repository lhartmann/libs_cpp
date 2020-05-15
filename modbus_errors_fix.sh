#! /bin/sh
exec > modbus_errors_by_name.h
cat << EOF
#ifndef MODBUS_ERRORS_BY_NAME_H
#include "modbus_errors.h"

/// Gets a user-friendly description for a Modbus error code.
inline const char *modbus_error_by_name(int err) {
	switch (err) {
EOF
sed -rn 's/^#define ([^ ]+)[ ]+[^ ]+ \/\/\!\< (.*)$/\tcase \1: return "\2"\;/p' < modbus_errors.h
cat << EOF
	}
	return "Unknown error.";
}
#endif
EOF
