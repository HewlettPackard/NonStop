# Import socket module 
import socket             
import unittest
import platform

def getMachineIP():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.connect(("8.8.8.8", 80))
    machinip=s.getsockname()[0]
    s.close()
    return machinip

def stest01():
    # Create a socket object
    s = socket.socket()

    # Define the port on which you want to connect
    port = 12345

    # connect to the server on local computer
    s.connect(('127.0.0.1', port))

    # receive data from the server
    print (s.recv(1024) )
    # close the connection
    s.close()
    return True

def stest02():
    # Create a socket object
    s = socket.socket()

    # Define the port on which you want to connect
    port = 12345

    # connect to the server on local computer
    ip=getMachineIP()
    s.connect((ip, port))

    # receive data from the server
    print (s.recv(1024) )
    # close the connection
    s.close()
    return True

class TestSum(unittest.TestCase):
    def test_getMachineIP(self):
        print("\nTest to validate the getMachineIP ")
        print("Machine Name :    " +str(platform.node()))
        reIP=getMachineIP()
        if "." in reIP:
            assert True == True
            print("Machine Name :    " +str(reIP))
        else:
            assert False == True
            print("Machine Name :    " +str(reIP))

    def test_client01(self):
        print("\nTest socket using 127.0.0.1 ")
        assert stest01() == True

    def test_client02(self):
        print("\nTest socket using < "+str(getMachineIP())+" > ")
        assert stest01() == True

if __name__ == '__main__':
    gV=None
    try:
        import pytest
        gV=True
    except ModuleNotFoundError:
        gV=False
        pass
    
    if gV == True:
        args=['-s', 'socketClient.py']
        pytest.main(args)
    else:
        unittest.main()
