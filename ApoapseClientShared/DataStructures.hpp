#pragma once
#include "TypeDefs.hpp"

static std::string GetDataStructures()
{
	static std::string scheme1 = R"(
{
	"custom_types": [
		{
			"name": "uuid",
			"underlying_type": "blob",
			"auto_fill": true
		},
		{
			"name": "username",
			"underlying_type": "blob"
		},
		{
			"name": "sha256",
			"underlying_type": "blob",
			"min_length": 32,
			"max_length": 32
		},
		{
			"name": "timestamp",
			"underlying_type": "integer",
			"min_length": 1
		},
		{
			"name": "datetime",
			"underlying_type": "text"
		}
	],

	"data_structures": [
		{
			"name": "empty",
			"fields": [	]
		},
		{
			"name": "error",
			"fields": [
				{
					"name": "error",
					"type": "integer",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "related_item",
					"type": "uuid",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "client_login",
			"fields": [
				{
					"name": "protocol_version",
					"type": "integer",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "username",
					"type": "username",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "password",
					"type": "sha256",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "sync",
			"fields": [
				{
					"name": "last_op_time",
					"type": "timestamp",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "install",
			"fields": [
				{
					"name": "server_name",
					"type": "text",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "admin_username",
					"type": "username",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "admin_password",
					"type": "sha256",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "server_setting",
			"fields": [
				{
					"name": "server_name",
					"type": "text",
					"required": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true,
						"client_ui": true
					}
				},
				{
					"name": "server_prefix",
					"type": "text",
					"required": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "operation",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": false
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": false
					}
				},
				{
					"name": "time",
					"type": "timestamp",
					"required": true,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": false
					}
				},							
				{
					"name": "ownership",
					"type": "integer",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": false
					}
				},				
				{
					"name": "related_user",
					"type": "username",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": false
					}
				},
				{
					"name": "item",
					"type": "integer",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": false
					}
				}
			]
		},
)";
	
static std::string scheme2 = R"(
		{
			"name": "user",
			"fields": [
				{
					"name": "username",
					"type": "username",
					"required": true,
					"unique": true,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "password",
					"type": "blob",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": false
					}
				},
				{
					"name": "password_salt",
					"type": "blob",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": false
					}
				},
				{
					"name": "is_temporary_password",
					"type": "bool",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": false
					}
				},
				{
					"name": "nickname",
					"type": "text",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "usergroup",
					"type": "uuid",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "avatar",
					"type": "blob",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "room",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "threads_layout",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				}
			]
		},
		{
			"name": "thread",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "parent_room",
					"type": "uuid",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				}
			]
		},
		{
			"name": "attachment",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "file_size",
					"type": "integer",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "parent_message",
					"type": "uuid",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "is_downloaded",
					"type": "bool",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": false
					}
				},
				{
					"name": "is_available",
					"type": "bool",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": true,
						"command": true
					}
				}
			]
		},
		{
			"name": "attachment_uuid",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "message",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "parent_thread",
					"type": "uuid",
					"required": false,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "direct_recipient",
					"type": "username",
					"required": false,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "message",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "sent_time",
					"type": "datetime",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "author",
					"type": "username",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "is_read",
					"type": "bool",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": true,
						"command": false
					}
				},
				{
					"name": "attachments",
					"type": "attachment",
					"required": false,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "tag",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "item_type",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				},
				{
					"name": "item_uuid",
					"type": "uuid",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": true,
						"client_storage": true,
						"command": true
					}
				}
			]
		},
		{
			"name": "mark_as_read",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "item_type",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "item_uuid",
					"type": "uuid",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "add_user",
			"fields": [
				{
					"name": "username",
					"type": "username",
					"required": true,
					"unique": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "temp_password",
					"type": "sha256",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "usergroup",
					"type": "uuid",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": false
					}
				}				
			]
		},
		{
			"name": "usergroup",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "name",
					"type": "text",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "permissions",
					"type": "text",
					"required": false,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": true,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "connected_user",
			"fields": [
				{
					"name": "user",
					"type": "username",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "user_status",
			"fields": [
				{
					"name": "user",
					"type": "username",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "status",
					"type": "integer",
					"required": true,
					"unique": false,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "server_info",
			"fields": [
				{
					"name": "status",
					"type": "text",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": true
					}
				},
				{
					"name": "file_stream_auth_code",
					"type": "blob",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": false
					}
				},
				{
					"name": "username",
					"type": "username",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": true
					}
				},
				{
					"name": "usergroups",
					"type": "usergroup",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": false
					}
				},
				{
					"name": "connected_users",
					"type": "connected_user",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": false
					}
				},
				{
					"name": "nickname",
					"type": "text",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": true
					}
				},
				{
					"name": "usergroup",
					"type": "uuid",
					"required": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": false
					}
				}
			]
		},
		{
			"name": "set_identity",
			"fields": [
				{
					"name": "password",
					"type": "sha256",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "nickname",
					"type": "text",
					"required": true,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				},
				{
					"name": "avatar",
					"type": "blob",
					"required": false,
					"unique": false,
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "attachment_download",
			"fields": [
				{
					"name": "uuid",
					"type": "uuid",
					"required": true,
					"unique": true,
					"read_permission": "all",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true
					}
				}
			]
		},
		{
			"name": "sync_info",
			"fields": [
				{
					"name": "nb_items",
					"type": "integer",
					"required": true,
					"unique": false,
					"read_permission": "self",
					"uses":	{
						"server_storage": false,
						"client_storage": false,
						"command": true,
						"client_ui": true
					}
				}
			]
		}
	]
}

)";

	return scheme1 + scheme2;
}