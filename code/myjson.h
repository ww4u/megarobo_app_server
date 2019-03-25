#ifndef MYJSON_H
#define MYJSON_H

#define __deload_double( obj, v, item )   if ( obj.contains( #item ) )\
                                { v.item = obj.value( #item ).toDouble(); logDbg()<<#item<<v.item; } \
                                else \
                                { return -1; }

#define _deload_double( obj, item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toDouble(); logDbg()<<#item<<var.item; } \
                                else \
                                { return -1; }
#define _deload_string( obj, item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toString(); logDbg()<<#item<<var.item; } \
                                else \
                                { return -1; }
#define _deload_int( obj, item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toInt(); logDbg()<<#item<<var.item; } \
                                else \
                                { return -1; }
#define _deload_bool( obj, item )   if ( obj.contains( #item ) )\
                                { var.item = obj.value( #item ).toBool(); logDbg()<<#item<<var.item; } \
                                else \
                                { logDbg()<<#item;return -1; }

#define _deload_json_obj( jsonObj, obj, item )   if ( obj.contains( #item ) )\
                                { jsonObj = obj.value( #item ).toObject(); logDbg()<<#item<<jsonObj; } \
                                else \
                                { return -1; }

//! deload
#define deload_double( item )   _deload_double( obj, item )
#define deload_string( item )   _deload_string( obj, item )
#define deload_int( item )      _deload_int( obj, item )
#define deload_bool( item )     _deload_bool( obj, item )

#define json_obj( item )    obj.insert( #item, var.item );

#endif // MYJSON_H
