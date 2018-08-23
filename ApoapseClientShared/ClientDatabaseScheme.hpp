#pragma once
#include "TypeDefs.hpp"

static std::string GetClientDbScheme()
{
	static std::string scheme = R"(
{
	"tables":
	[
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
				}
			]
		}
	]
}
	)";

	return scheme;
}