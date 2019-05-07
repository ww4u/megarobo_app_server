import json
import socket as SOCKET

import time
import math

def doSomething( socket, dict ):
    dat = json.dumps(dict) + '#'
    socket.sendall( dat.encode() )
    
    if ( "command" in dict.keys() and dict["command"] == "query" ):
        data = socket.recv( 2048 )
        print( data.decode() )
        return data.decode()
    else:
        pass 
        # time.sleep( 0.1 )                

class MegaRobo():
    def __init__( self, ip="127.0.0.1", port=50000 ):

        self.mSocket1 = SOCKET.socket()
        # self.mSocket2 = SOCKET.socket()
        # self.mSocket3 = SOCKET.socket()

        ipPort=( ip, port )
        self.mSocket1.connect( (ip,port) )
        # self.mSocket2.connect( (ip,port+1) )
        # self.mSocket3.connect( (ip,port+2) )

    # do the command
    def doCmd( self, dict, socket = None ):
        socket = socket if socket is not None else self.mSocket1
        dat = json.dumps(dict) + '#'
        socket.sendall( dat.encode() )
        
        if ( "command" in dict.keys() and dict["command"] == "query" ):
            # data = socket.recv( 2048 )
            # print( data.decode() )
            # return data.decode()
            pass
        else:
            pass 
            # time.sleep( 0.1 )

    def doRecv( self, socket=None ):
        socket = socket if socket is not None else self.mSocket1
        data = socket.recv( 2048 )
        return data.decode()  

    def status( self ):
        query_device_status = {
        "command":"query",
        "item": "device_status"
        }

        self.doCmd( query_device_status )
        return self.doRecv()

    def query( self, x ):
        querycmd = {
        "command":"query",
        "item": x
        }

        self.doCmd( querycmd )
        return self.doRecv()

    def request( self, x ):
        querycmd = {
        "command":"request",
        "item": x
        }
        self.doCmd( querycmd )

    def waitX( self, sth, tmo = 120 ):
        while ( tmo > 0 ):            
            stat = self.status( )
            # print( stat )
            if( sth in stat ):
                return True 
            else:
                pass
            tmo -= 1
            time.sleep(  1 )
        raise Exception("time out")            
        return False     

    def waitIdle( self, tmo = 120 ):
        return self.waitX( "stoped" )               

class RoboT4( MegaRobo ):
    def __init__( self, ip="127.0.0.1", port=50000 ):
        MegaRobo.__init__( self, ip, port )

    def step( self, angle, z = 0, cont = False ):
        stepCmd = { "command": "step",
              "angle": angle,
              "z": z,
              "continous": cont
            }

        self.doCmd( stepCmd )

    def jStep( self, j, angle, cont = False ):        
        """
        j: joint
        angle: angle or step
        """
        joint_step = {
        "command":"joint_step",
        "value":1,
        "continous": cont,
        "joint": 3 
        }

        joint_step["joint"] = j
        joint_step["value"] = angle 
        
        self.doCmd( joint_step )

    def config( self ):
        _config = {
        "command": "config",
        "step": 1,
        "joint_step": 2,
        "speed": 0.2
        }  
        self.doCmd( _config )      

    def status( self ):
        query_device_status = {
        "command":"query",
        "item": "device_status"
        }

        self.doCmd( query_device_status )
        return self.doRecv()

    # def waitIdle( self, tmo = 120 ):
    #     while ( tmo > 0 ):            
    #         stat = self.status( )
    #         if( "stoped" in stat ):
    #             return True 
    #         else:
    #             pass
    #         tmo -= 1
    #         time.sleep(  1 )
    #     raise Exception("time out")            
    #     return False 

    def doAction( self, act ):
        action = {
                "command": "action",
                "item": act 
        }
        self.doCmd( action )

    # def query( self, sth ):
    #     query_arg = {
    #     "command" : "query",
    #     "item": sth
    #     }   
    #     self.doCmd( query_arg )
    #     return self.doRecv()

class LetRobo( MegaRobo ):
    def __init__( self, ip="127.0.0.1", port = 2345 ):
        MegaRobo.__init__( self, ip, port )

    def request_home( self, v = 10 ):
        localVar = {
            "command": "request",
            "item": "home",
            "velocity": v
        }
        self.doCmd( localVar )

    def request_to( self, x, y, z, v = 10  ):
        localVar = {
            "command": "request",
            "item": "to",
            "velocity": v,
            "x": x,
            "y": y,
            "z": z
        }
        self.doCmd( localVar )                

    def request_step( self, x, y, z, n = 1 ):
        localVar = {
            "command": "request",
            "item": "step",
            "velocity": 10,
            "x": x,
            "y": y,
            "z": z,
            "n": n 
        }
        self.doCmd( localVar )    

    def request_origin( self, x, y, z, v ):
        localVar = {
            "command": "request",
            "item": "origin",
            "velocity": v,
            "x": x,
            "y": y,
            "z": z
        }
        self.doCmd( localVar )   

    def request_traceX( self, type="zigzagx",v=10, x=1, y=1, z= 0 ):
        localVar = {
            "command": "request",
            "item": type,
            "velocity": v,
            "x": x,
            "y": y,
            "z": z,
        }
        self.doCmd( localVar )            

    def request_continue( self ):
        localVar = {
            "command": "request",
            "item": "continue"
        }
        self.doCmd( localVar )            

    def config_rst( self ):
        localVar = {
            "command": "config",
            "item":"rst",
        }        
        self.doCmd( localVar ) 

    def config_orig( self, ox, oy, oz ):
        localVar = {
            "command": "config",
            "item":"origin",
            "x": ox, 
            "y": oy, 
            "z": oz, 
        }        
        self.doCmd( localVar ) 

    def config_whz( self, w, h, z ):
        localVar = {
            "command": "config",
            "item":"whz",
            "w": w, 
            "h": h, 
            "d": z, 
        }        
        self.doCmd( localVar )           

    def config_dwdhdz( self, dw, dh, dz ):
        localVar = {
            "command": "config",
            "item":"dwdhdz",
            "dw": dw, 
            "dh": dh, 
            "dd": dz, 
        }        
        self.doCmd( localVar )           

    def config_rv( self,  rv):
        localVar = {
            "command": "config",
            "item":"rv",
            "rv": rv
        }        
        self.doCmd( localVar )           

    def status( self ):
        query_device_status = {
        "command":"query",
        "item": "status"
        }

        self.doCmd( query_device_status )
        return self.doRecv()

    def pose( self ):
        jsondata = self.query( "pose" )        
        obj = json.loads( jsondata )
        return ( obj["x"], obj["y"], obj["z"], obj["vx"], obj["vy"], obj["vz"] )

def let_check_pos( robo, pos, err = 1, api=None, para=None ):

    if ( api != None ):
        if ( para ):
            api( para )
        else:
            api()            

    posN = robo.pose()
    dist = 0
    for i in range( 3 ):
        dist += pow( pos[i]-posN[i], 2 )
    dist = math.sqrt( dist )
    if ( dist > err ):
        print( pos )
        print( posN )
        assert(False)
    else:
        pass         

def let_test_rst( robo ):
    robo.config_orig( 0,0,0 )
    robo.request_home( 50 )
    robo.waitIdle()

def let_test_home( robo ):

    def moveProc():
        robo.request_to( 3,5,2 )
        robo.waitIdle()

    def localHome( speed ):
        robo.request_home( speed )
        robo.waitIdle()

    moveProc()

    # home1
    # check pos
    let_check_pos( robo, (0,0,0), api=localHome, para=( 10 ) )

    let_check_pos( robo, (3,5,1), api = moveProc )

    # home2
    let_check_pos( robo, (0,0,0), api=localHome, para=( 1 ) )

def let_test_origin( robo ):

    def moveProc( speed ):
        robo.request_origin( 10,20,3, speed )
        robo.waitIdle()
    
    let_check_pos( robo, (0,0,0), api=moveProc, para=( 10 ) )

    let_check_pos( robo, (0,0,0), api=moveProc, para=( 5 ) )

def let_test_to( robo ):
    robo.request_to( 5,4,6 )
    robo.waitIdle( )

    let_check_pos( robo, (5,4,6) )

def let_test_step( robo ):
    let_test_rst( robo )

    # check x
    robo.request_step( 10,0,0 )
    robo.waitIdle()
    let_check_pos( robo, (10,0,0) )

    # check y
    robo.request_step( 0,15,0 )
    robo.waitIdle()
    let_check_pos( robo, (10,15,0) )

    # check z
    robo.request_step( 0,0,5 )
    robo.waitIdle()
    let_check_pos( robo, (10,15,5) )

def let_test_stepn( robo ):
    let_test_rst( robo )

    robo.request_step( 10,0,0, 3 )
    robo.waitX( "pending" )
    let_check_pos( robo, (10,0,0 ) )

    robo.request_continue()
    robo.waitX( "pending" )
    let_check_pos( robo, (20,0,0) )

    robo.request_continue()
    robo.waitIdle()
    let_check_pos( robo, (30,0,0) )

    # step again, normal stop
    let_test_rst( robo )

    robo.request_step( 10,0,0, 3 )
    robo.waitX( "pending" )
    let_check_pos( robo, (10,0,0 ) )

    robo.request( "stop" )
    robo.waitIdle()

    # step again, e stop
    let_test_rst( robo )

    robo.request_step( 10,0,0, 3 )
    robo.waitX( "pending" )
    let_check_pos( robo, (10,0,0 ) )

    robo.request( "estop" )
    robo.waitIdle()

def let_test_zigzag_x( robo ):
    let_test_rst( robo )
    robo.request_traceX( type="zigzagx" )    
    robo.waitIdle()

    let_check_pos( robo, (50,60,0) )

def let_test_zigzag_y( robo ):
    let_test_rst( robo )
    robo.request_traceX( type="zigzagy" )    
    robo.waitIdle()

    let_check_pos( robo, (50,60,0) )

def let_test_snake_x( robo ):
    let_test_rst( robo )
    robo.request_traceX( type="snakex" )    
    robo.waitIdle()

    let_check_pos( robo, (50,60,0) )

def let_test_snake_y( robo ):
    let_test_rst( robo )
    robo.request_traceX( type="snakey" )    
    robo.waitIdle()

    let_check_pos( robo, (0,60,0) )

def let_test_slope(robo ):
    let_test_rst( robo )
    robo.request_traceX( type="slope", x=40,y=60 )    
    robo.waitIdle()

    let_check_pos( robo, (40,60,0) )

def testMain( robo ):
    let_test_rst( robo )

    # let_test_home( robo ) 

    # let_test_to( robo )

    # let_test_step( robo )

    # let_test_stepn( robo )

    # let_test_zigzag_x( robo )
    # let_test_zigzag_y( robo )
    # let_test_snake_x( robo )
    # let_test_snake_y( robo )

    # let_test_slope( robo )

    print( robo.pose() )

if __name__=="__main__":
# def roboFlow():

    robo = LetRobo( )
    print( robo.pose() )

    testMain( robo )

    exit( 0 )

    # let_test_home( robo )

    # assert( False )

    # robo.request_home( 5 )
    # robo.waitIdle( )

    # query
    # robo.config_rst()
    # robo.config_whz( 30,60,20 )
    # robo.config_dwdhdz( 5,6,4)
    # robo.config_rv( 10 )
    # robo.config_orig( 10,20,5 )

    print( robo.query( "config" ) )
    print( robo.query( "pose" ) )
    print( robo.query( "status" ) )

    # config


    # robo.request_zigzagX()
    # robo.request_traceX( "zigzagx")
    # robo.request_traceX( "zigzagy")
    # robo.request_traceX( "snakex")
    # robo.request_traceX( "snakey")
    # robo.request_traceX( type = "slope", v = 10, x = 50, y=50, z=0 )
    # robo.waitIdle( )

    robo.request_to( 3, 5, 6 )
    # robo.request( "stop" )
    robo.waitIdle( )
    print( robo.query( "pose") )

    # robo.request_step(10,0,0, 5 )

    # robo.request_continue()
    # print( robo.query_x( "status") )
    # robo.request_continue()
    # robo.request_continue()
    # robo.request_continue()
    # print( robo.query_x( "status") )

    # print( robo.query_x( "pose") )

    # robo.config_orig( 1,2,3)

    # print( robo.query_x( "config") )
    # print( robo.query_x( "status") )

    # robo = RoboT4( ip="127.0.0.1")
    # print( robo.query( "link_status" ) )
    time.sleep( 10 )
    exit( 0 )

    # for i in range( 100000 ):
    #     print( robo.status() )

    # print ( robo.doRecv() )
    # print ( robo.doRecv() )

    # time.sleep( 60 )

    # tSlp = 0.1
    # for i in range( 1000 ):
    #     robo.step( 0 )
    #     # time.sleep( tSlp )
    #     robo.step( 90 )
    #     # time.sleep( tSlp )
    #     robo.step( 180 )
    #     # time.sleep( tSlp )
    #     robo.step( 270 )
    #     # time.sleep( tSlp )

    # time.sleep( 10 )

    robo.step( 0 )
    assert(False) 

    # for i in range( 100000 ):
    #     var = robo.query( "meta" );print( var )
    #     time.sleep( 5 )
    #     print( i )
    # assert(False)        

    # var = robo.query( "exception" );print( var )
    
    # var = robo.query( "pose" );print( var )
    # var = robo.query( "parameter" );print( var )
    # var = robo.query( "config" );print( var )
    # var = robo.query( "dataset" );print( var )

    # robo.doAction("home")
    # robo.jStep( 3, 90 )
    # robo.jStep( 3, -1, True )
    # robo.waitIdle( )  

    robo.step( 270, 0, True )

    time.sleep( 2 )
    robo.doAction( "stop" )

    # for i in range( 1000 ):
    #     if i % 2 == 0:
    #         robo.step( 0, 1 )
    #     else:
    #         robo.step( 0, -1 )                        
    #     robo.waitIdle( )

    # for i in range( 100 ):
    #     if i % 2 == 0:
    #         robo.step( 90, 0 )
    #     else:
    #         robo.step( 270, 0 )                        
    #     robo.waitIdle( )        
    
    # for i in range( 2 ):
    #     if i % 2 == 0:
    #         robo.jStep( 3, 180 )
    #     else:
    #         pass 
    #         # robo.jStep( 3, 180 )                        
    #     robo.waitIdle( )           
# if __name__=="__main__":
def testFlow():
    _mSocket = SOCKET.socket() 

    _mSocket1 = SOCKET.socket() 
    _mSocket2 = SOCKET.socket() 

    # connect
    ip = "127.0.0.1"
    # port = 2345
    port = 50000
    ipPort = ( ip, port )
    _mSocket.connect( ipPort )

    _mSocket1.connect( (ip,port+1) )
    _mSocket2.connect( (ip,port+2) )

    # _mSocket.send( "*hello".encode() )
    # data = _mSocket.recv( 1024 )
    # print( data.decode() )

    step = { "command": "step",
              "angle": 90,
              "z": 0,
              "continous": False
            }
    # doSomething( _mSocket, step )
                
    # print( json.dumps(step) )     
    # dat = json.dumps(step)
    # _mSocket.send( dat.encode() )

    joint_step = {
        "command":"joint_step",
        "value":1,
        "continous":False,
        "joint": 3 
    }
    # doSomething( _mSocket, joint_step ) 

    config = {
        "command": "config",
        "step": 1,
        "joint_step": 2,
        "speed": 0.2
    }
    # doSomething( _mSocket, config ) 

    action = {
                "command": "action",
                "item": "home"
        }
    # doSomething( _mSocket, action )     

    # rep_action = {
    #             "command": "action",
    #             "item": "test",
    #             "data" : 1
    #     }   
    # for i in range( 10 ):
    #      rep_action["data"] = i 
    #      doSomething( _mSocket, rep_action )            

    indicator = {
        "command":"indicator",
        "status": False
    }
    # doSomething( _mSocket, indicator )     

    add_pose = {
        "command":"add",
        "name": "point2",
        "pose":{
            "x": 1,
            "y": 2,
            "z": 3,
            "w": 4,
            "h": 5
        }
    }
    doSomething( _mSocket, add_pose )  

    time.sleep( 1 )

    # querys
    query_device_status = {
        "command":"query",
        "item": "device_status"
    }
    # doSomething( _mSocket, query_device_status ) 

    # time.sleep( 10 )
    # doSomething( _mSocket, query ) 

    set_link = {
        "command":"link_status",
        "status": "idle"
    }
    # doSomething( _mSocket, set_link ) 

    # time.sleep( 2 )

    query_link = {
        "command":"query",
        "item": "link_status"
    }
    # doSomething( _mSocket, query_link ) 

    query_exception={
        "command":"query",
        "item":"exception"
    }
    # doSomething( _mSocket, query_exception ) 

    query_meta = {
        "command":"query",
        "item": "meta"
    }
    # doSomething( _mSocket, query_meta ) 

    query_pose = {
        "command": "query",
        "item": "pose"
    }
    # doSomething( _mSocket, query_pose ) 

    query_para = {
        "command" : "query",
        "item": "parameter"
    }
    # doSomething( _mSocket, query_para ) 

    query_config = {
        "command" : "query",
        "item": "config"
    }
    # doSomething( _mSocket, query_config ) 

    query_dataset = {
        "command": "query",
        "item": "dataset"
    }
    # doSomething( _mSocket, query_dataset ) 

    # data = _mSocket.recv( 1024 )
    # print( data.decode() )

    # socket1
    # doSomething( _mSocket1, query ) 
    # data = _mSocket1.recv( 1024 )
    # print( data.decode() )

    def waitIdle( tmo = 60 ):
        while ( tmo > 0 ):            
            stat = doSomething( _mSocket, query_device_status ) 
            if( "stoped" in stat ):
                return True 
            else:
                pass
            tmo -= 1
            time.sleep(  1 )
        raise Exception("time out")            
        return False 

    # waitIdle( )      
    # time.sleep( 20 )   
    # doSomething( _mSocket, query_device_status )                   

