#ifndef MYJSON_H
#define MYJSON_H

#define __deload_double( obj, v, item )   if ( obj.contains( #item ) )\
                                { v.item = obj.value( #item ).toDouble(); logDbg()<<#item<<v.item; } \
                                else \
                                { return -1; }

#define _try_deload_xx( obj, item, type, def )  \
                                                var.item=def;\
                                                if ( obj.contains( #item ) )\
                                                { var.item = obj.value( #item ).to##type(); logDbg()<<#item<<var.item; \
                                                  var.bmMap.insert( #item, true );\
                                                }

#define _try_deload_double( obj, item, def )     _try_deload_xx( obj, item, Double, def )\
                                                 else{ var.bmMap.insert( #item, false); }

#define _deload_double( obj, item )   _try_deload_xx( obj, item, Double, 0 )\
                                      else{ return -1; }

#define _deload_string( obj, item )   _try_deload_xx( obj, item, String, "" )\
                                     else{ return -1; }
#define _deload_int( obj, item )    _try_deload_xx( obj, item, Int, 0 )\
                                    else{ return -1; }
#define _deload_bool( obj, item )    _try_deload_xx( obj, item, Bool, false )\
                                    else{ return -1; }

#define _deload_json_obj( jsonObj, obj, item )   if ( obj.contains( #item ) )\
                                { jsonObj = obj.value( #item ).toObject(); logDbg()<<#item<<jsonObj; } \
                                else \
                                { return -1; }

#define _has_item( t ) ( var.bmMap.contains( #t ) && var.bmMap.value( #t ) )

//! deload
#define deload_double( item )   _deload_double( obj, item )
#define try_deload_double( item )   _try_deload_double( obj, item, 0 )

#define deload_double2s( item1, item2 )   _deload_double( obj, item1 )\
                                          _deload_double( obj, item2 )
#define deload_double3s( item1, item2, item3 )   deload_double2s( item1, item2 )\
                                                 _deload_double( obj, item3 )


#define deload_string( item )   _deload_string( obj, item )

#define deload_int2s( item1, item2 )      _deload_int( obj, item1 )\
                                            _deload_int( obj, item2 )

#define deload_int( item )      _deload_int( obj, item )
#define deload_bool( item )     _deload_bool( obj, item )

#define json_obj( item )    obj.insert( #item, var.item );
#define json_obj2s( item1, item2 )    json_obj( item1 );\
                                      json_obj( item2 )
#define json_obj3s( item1, item2, item3 )    json_obj2s( item1, item2 );\
                                             json_obj( item3 )

#endif // MYJSON_H
