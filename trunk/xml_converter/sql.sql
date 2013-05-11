
CREATE TABLE "devices"
(
	"id" INTEGER PRIMARY KEY AUTOINCREMENT,
	"name" TEXT 
);

CREATE TABLE "device_register_types"
(
	"id" INTEGER PRIMARY KEY AUTOINCREMENT,
	"name" TEXT 
);


CREATE TABLE "devices_registers"
(
	"id" INTEGER PRIMARY KEY AUTOINCREMENT,
	"device_id" INT  ,
	"type_id" INT 
);

CREATE TABLE "bitfields_enums"
(
	"id" INTEGER PRIMARY KEY AUTOINCREMENT,
	"name" TEXT  ,
	"mask" INT  ,
	"value" INT 
);


CREATE TABLE "bitfields"
(
	"id" INTEGER PRIMARY KEY AUTOINCREMENT,
	"enum_id" INT  ,
	"mask" INT  ,
	"register_id" INT 
);


