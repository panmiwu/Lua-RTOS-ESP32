/*
 * Lua RTOS, Lua helper functions for throw an error from a driver error
 *
 * Copyright (C) 2015 - 2017
 * IBEROXARXA SERVICIOS INTEGRALES, S.L.
 *
 * Author: Jaume Olivé (jolive@iberoxarxa.com / jolive@whitecatboard.org)
 *
 * All rights reserved.
 *
 * Permission to use, copy, modify, and distribute this software
 * and its documentation for any purpose and without fee is hereby
 * granted, provided that the above copyright notice appear in all
 * copies and that both that the copyright notice and this
 * permission notice and warranty disclaimer appear in supporting
 * documentation, and that the name of the author not be used in
 * advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * The author disclaim all warranties with regard to this
 * software, including all implied warranties of merchantability
 * and fitness.  In no event shall the author be liable for any
 * special, indirect or consequential damages or any damages
 * whatsoever resulting from loss of use, data or profits, whether
 * in an action of contract, negligence or other tortious action,
 * arising out of or in connection with the use or performance of
 * this software.
 */

#include "error.h"
#include "lauxlib.h"

#include <string.h>
#include <stdlib.h>

#include <sys/driver.h>

int luaL_driver_error(lua_State* L, driver_error_t *error) {
	driver_error_t err;
    int ret_val;
    
    memcpy(&err, error, sizeof(driver_error_t));
    
    if (err.type == LOCK) {
        ret_val = luaL_error(L,
            "%s%d is used by %s%d",
			err.lock_error->target_driver->name,
			err.lock_error->target_unit,
			err.lock_error->lock->owner->name,
			err.lock_error->lock->unit
		);
        
        free(err.lock_error);
        free(error);

        return ret_val;
    } else if (err.type == OPERATION) {
    	if (err.msg) {
            ret_val = luaL_error(L,
                "%d:%s (%s)",
    			err.exception,
    			driver_get_err_msg(&err),
                err.msg
            );
    	} else {
            ret_val = luaL_error(L,
                "%d:%s",
    			err.exception,
    			driver_get_err_msg(&err)
            );
    	}
    }
    
    int rc = luaL_error(L, driver_get_err_msg(error));
    free(error);

    return rc;
}

int luaL_deprecated(lua_State* L, const char *old, const char *new) {
	// Build the deprecated message
	luaL_where(L, 1);
	lua_pushfstring(L, "WARNING %s is deprecated, please use %s instead\r\n", old, new);
	lua_concat(L, 2);

	printf(luaL_checkstring (L, -1));

	// Remove string from the stack
	lua_pop(L,1);

	return 0;
}
