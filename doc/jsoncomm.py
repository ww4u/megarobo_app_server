import json
import socket as SOCKET

import time

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

class RoboT4():
    def __init__( self, ip="127.0.0.1", port=50000 ):

        self.mSocket1 = SOCKET.socket()
        self.mSocket2 = SOCKET.socket()
        self.mSocket3 = SOCKET.socket()

        ipPort=( ip, port )
        self.mSocket1.connect( (ip,port) )
        self.mSocket2.connect( (ip,port+1) )
        self.mSocket3.connect( (ip,port+2) )

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

    def status( self ):
        query_device_status = {
        "command":"query",
        "item": "device_status"
        }

        self.doCmd( query_device_status )
        return self.doRecv()

    def waitIdle( self, tmo = 120 ):
        while ( tmo > 0 ):            
            stat = self.status( )
            if( "stoped" in stat ):
                return True 
            else:
                pass
            tmo -= 1
            time.sleep(  1 )
        raise Exception("time out")            
        return False 

    def doAction( self, act ):
        action = {
                "command": "action",
                "item": act 
        }
        self.doCmd( action )

    def query( self, sth ):
        query_arg = {
        "command" : "query",
        "item": sth
        }   
        self.doCmd( query_arg )
        return self.doRecv()

if __name__=="__main__":
# def roboFlow():
    # robo = RoboT4( )
    # robo = RoboT4( ip="169.254.1.2" )
    # robo = RoboT4( ip="127.0.0.1", port=2345 )
    robo = RoboT4( ip="127.0.0.1")

    # for i in range( 100000 ):
    #     print( robo.status() )

    # print ( robo.doRecv() )
    # print ( robo.doRecv() )

    # time.sleep( 60 )

    tSlp = 0.1
    for i in range( 1000 ):
        robo.step( 0 )
        # time.sleep( tSlp )
        robo.step( 90 )
        # time.sleep( tSlp )
        robo.step( 180 )
        # time.sleep( tSlp )
        robo.step( 270 )
        # time.sleep( tSlp )

    # time.sleep( 10 )

    assert(False) 

    for i in range( 100000 ):
        var = robo.query( "meta" );print( var )
        time.sleep( 5 )
        print( i )
    assert(False)        

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

