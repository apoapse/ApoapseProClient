// ----------------------------------------------------------------------------
// Copyright (C) 2020 Apoapse
// Copyright (C) 2020 Guillaume Puyal
//
// Distributed under the Apoapse Pro Client Software License. Non-commercial use only.
// See accompanying file LICENSE.md
//
// For more information visit https://github.com/apoapse/
// And https://apoapse.space/
// ----------------------------------------------------------------------------

#pragma once
#include "TypeDefs.hpp"

static std::string GetClientDbScheme()
{
	static std::string scheme = R"(
{
	"tables":
	[
		{
			"name": "users",
			"fields":
			[
				{
					"name": "id",
					"type": "integer",
					"primary": true
				},		
				{
					"name": "username",
					"type": "blob",
					"unique": true
				},
				{
					"name": "nickname",
					"type": "text"
				}
			]
		},
		{
			"name": "operations",
			"fields":
			[
				{
					"name": "id",
					"type": "integer",
					"primary": true
				},		
				{
					"name": "type",
					"type": "blob"
				},
				{
					"name": "time",
					"type": "integer"
				},
				{
					"name": "item_dbid",
					"type": "integer"
				},
				{
					"name": "ownership",
					"type": "blob"
				},
				{
					"name": "related_user",
					"type": "blob"
				}
			]
		},
		{
			"name": "local_settings",
			"fields":
			[
				{
					"name": "name",
					"type": "text",
					"unique": true
				},
				{
					"name": "value",
					"type": "text"
				}
			]
		},
		{
			"name": "rooms",
			"fields":
			[
				{
					"name": "id",
					"type": "integer",
					"primary": true
				},		
				{
					"name": "uuid",
					"type": "blob",
					"unique": true
				},		
				{
					"name": "name",
					"type": "text"
				},
				{
					"name": "unread_msg_count",
					"type": "integer",
					"defaultValue": 0,
					"canBeNull": true
				}
			]
		},
		{
			"name": "threads",
			"fields":
			[
				{
					"name": "id",
					"type": "integer",
					"primary": true
				},
				{
					"name": "room_uuid",
					"type": "blob"
				},		
				{
					"name": "uuid",
					"type": "blob",
					"unique": true
				},
				{
					"name": "name",
					"type": "text"
				}
			]
		},
		{
			"name": "messages",
			"fields":
			[
				{
					"name": "id",
					"type": "integer",
					"primary": true
				},
				{
					"name": "uuid",
					"type": "blob",
					"unique": true
				},		
				{
					"name": "thread_uuid",
					"type": "blob"
				},
				{
					"name": "author",
					"type": "blob"
				},
				{
					"name": "sent_time",
					"type": "text"
				},
				{
					"name": "content",
					"type": "blob"
				},
				{
					"name": "is_read",
					"type": "integer"
				}
			]
		}
	]
}
	)";

	return scheme;
}