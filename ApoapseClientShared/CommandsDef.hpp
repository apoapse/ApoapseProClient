#pragma once
#include "TypeDefs.hpp"

static std::string GetCommandDef()
{
	static std::string scheme = R"(

{
	"commands":	[
		{
			"name": "error",
			"name_short": "ee",
			"datastructure": "error",
			"require_authentication": false,
			"only_non_authenticated": false,
			"reception": {
				"server": false,
				"client": true
			},
			"operation": {
				"register": false
			}
		},
		{
			"name": "version",
			"name_short": "gv",
			"datastructure": "empty",
			"require_authentication": false,
			"only_non_authenticated": false,
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": false
			}
		},
		{
			"name": "install",
			"name_short": "in",
			"datastructure": "install",
			"require_authentication": false,
			"only_non_authenticated": true,
			"reception": {
				"server": true,
				"client": false
			},
			"operation": {
				"register": false
			}
		},
		{
			"name": "login",
			"name_short": "lc",
			"datastructure": "client_login",
			"require_authentication": false,
			"only_non_authenticated": true,
			"reception": {
				"server": true,
				"client": false
			},
			"operation": {
				"register": false
			}
		},
		{
			"name": "server_settings",
			"name_short": "ss",
			"datastructure": "server_setting",
			"require_authentication": false,
			"only_non_authenticated": true,
			"reception": {
				"server": false,
				"client": true
			},
			"client_ui": {
				"propagate": true,
				"signal_name": "OnUpdatedServerSettings"
			}
		},
		{
			"name": "server_info",
			"name_short": "si",
			"datastructure": "server_info",
			"require_authentication": false,
			"only_non_authenticated": true,
			"reception": {
				"server": false,
				"client": true
			},
			"client_ui": {
				"propagate": true,
				"signal_name": "OnReceivedServerInfo"
			}
		},
		{
			"name": "request_sync",
			"name_short": "rs",
			"datastructure": "sync",
			"require_authentication": true,
			"reception": {
				"server": true,
				"client": false
			},
			"propagation": {
				"propagate": false,
				"exclude_self": false
			}
		},
		{
			"name": "user",
			"name_short": "nu",
			"datastructure": "user",
			"require_authentication": true,
			"only_non_authenticated": false,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": false,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "all"
			}
		},
		{
			"name": "create_room",
			"name_short": "cr",
			"datastructure": "room",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "all"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "create_thread",
			"name_short": "ct",
			"datastructure": "thread",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "all"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "new_message",
			"name_short": "cm",
			"datastructure": "message",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "all"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "direct_message",
			"name_short": "dm",
			"datastructure": "message",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "self"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "add_tag",
			"name_short": "at",
			"datastructure": "tag",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": true
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "self"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "add_user",
			"name_short": "au",
			"datastructure": "add_user",
			"require_authentication": true,
			"required_permissions": "CREATE_USER",
			"reception": {
				"server": true,
				"client": false
			},
			"operation": {
				"register": false,
				"ownership": "self"
			},
			"propagation": {
				"propagate": false,
				"exclude_self": false
			}
		},
		{
			"name": "set_identity",
			"name_short": "fp",
			"datastructure": "set_identity",
			"require_authentication": true,
			"only_temporary_auth": true,
			"reception": {
				"server": true,
				"client": false
			}
		},
		{
			"name": "mark_as_read",
			"name_short": "mr",
			"datastructure": "mark_as_read",
			"require_authentication": true,
			"save_on_receive": {
				"server": true,
				"client": false
			},
			"reception": {
				"server": true,
				"client": true
			},
			"operation": {
				"register": true,
				"ownership": "self"
			},
			"propagation": {
				"propagate": true,
				"exclude_self": false
			}
		},
		{
			"name": "change_user_status",
			"name_short": "us",
			"datastructure": "user_status",
			"require_authentication": true,
			"save_on_receive": {
				"server": false,
				"client": false
			},
			"reception": {
				"server": false,
				"client": true
			},
			"operation": {
				"register": false,
				"ownership": "self"
			},
			"propagation": {
				"propagate": false,
				"exclude_self": false
			}
		},
		{
			"name": "ready_to_receive_file",
			"name_short": "rr",
			"datastructure": "empty",
			"require_authentication": true,
			"save_on_receive": {
				"server": false,
				"client": false
			},
			"reception": {
				"server": false,
				"client": true
			},
			"operation": {
				"register": false
			},
			"propagation": {
				"propagate": false,
				"exclude_self": false
			}
		},
		{
			"name": "attachment_download",
			"name_short": "ad",
			"datastructure": "attachment_download",
			"require_authentication": true,
			"save_on_receive": {
				"server": false,
				"client": false
			},
			"reception": {
				"server": true,
				"client": false
			},
			"operation": {
				"register": false
			},
			"propagation": {
				"propagate": false,
				"exclude_self": false
			}
		},
		{
			"name": "attachment_available",
			"name_short": "av",
			"datastructure": "attachment_uuid",
			"require_authentication": true,
			"save_on_receive": {
				"server": false,
				"client": false
			},
			"reception": {
				"server": false,
				"client": true
			},
			"operation": {
				"register": true,
				"datastructure": "attachment"
			}
		},
		{
			"name": "start_sync",
			"name_short": "sy",
			"datastructure": "sync_info",
			"require_authentication": true,
			"reception": {
				"server": false,
				"client": true
			},
			"client_ui": {
				"propagate": true,
				"signal_name": "OnReceived"
			}
		}
	]
}

)";

	return scheme;
}