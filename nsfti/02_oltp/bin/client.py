#
#  Copyright 2021 Hewlett Packard Enterprise Development LP
#
#    ALL RIGHTS RESERVED
#

import os
import socket
import sys
import argparse
import configparser
import uuid
import string
import random


def read_configuration(config_file):
    temp_dict = {}
    config = configparser.ConfigParser()
    config.optionxform = str
    with open(config_file, 'r') as f:
        config.read_string('[config]\n' + f.read())

        # get sections and iterate over each
        sections = config.sections()

        for section in sections:
            options = config.options(section)
            for option in options:
                temp_dict[option] = config.get(section, option)
    return temp_dict


def main():
    config_file = "./config.properties"
    parser = argparse.ArgumentParser()
    parser.add_argument("--file")
    parser.add_argument("--command")
    parser.add_argument("--key")
    parser.add_argument("--value")

    args = parser.parse_args()

    if args.file:
        config_file = args.file

    # Read the configuration file
    config_dict = read_configuration(config_file)
    oltp_client = WorkloadGenerator(config_dict)
    if args.command == "INSERT":
        print("Inserting key " + args.key)
        status = oltp_client.insert(args.key,args.value)
        if status == 0:
            print("INSERT command is successful.")
    elif args.command == "READ":
        print("Reading value for key " + args.key)
        output = oltp_client.read(args.key)
        print("Value= " + output)
    elif args.command == "DELETE":
        print("Deleting key " + args.key)
        status = oltp_client.delete(args.key)
        if status == 0:
            print("DELETE command is successful.")
    elif args.command == "UPDATE":
        print("Updating key " + args.key)
        status = oltp_client.insert(args.key,args.value)
        if status == 0:
            print("UPDATE command is successful.")
    else:
        oltp_client.start()


def convert_to_int(value):
    units = {"K": 1000, "M": 1000000}
    size = 0
    try:
        size = int(value)
    except ValueError:
        unit = value[-1]  # get the letter
        n = int(value[:-1])  # convert all but the letter
        size = (n * units[unit])
    return size


def find_gcd(x, y):
    while (y):
        x, y = y, x % y

    return x


def generate_msg(msg_size):
    value = ''.join(random.choices(string.ascii_uppercase +
                                   string.digits, k=msg_size))
    return value


def generate_key():
    return str(uuid.uuid1())


class WorkloadGenerator:

    def __init__(self, config):
        self.config = config
        self.msg_size = convert_to_int(self.config['MSG_SIZE'])
        self.msg = generate_msg(self.msg_size)
        self.CRLF = "\r\n"
        self.keys = []
        self.socket_fd = -1
        self.connect_to_server(False)

    def connect_to_server(self, retry):
        try:
            self.socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        except socket.error as err:
            print("socket creation failed with error %s" % (err))
        # print (self.config['HOST'], int(self.config['PORT']))
        if not retry:
            self.socket_fd.connect((self.config['HOST'], int(self.config['PORT'])))
            return

        while retry:
            try:
                self.socket_fd.connect((self.config['HOST'], int(self.config['PORT'])))
                return
            except OSError:
                self.socket_fd.close()
                self.socket_fd = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
                print("Connection refused. Retrying..")

    def calculate_workload_count(self):
        gcd = find_gcd(find_gcd(int(self.config['INSERT_PERCENTAGE']), int(self.config['READ_PERCENTAGE'])),
                       int(self.config['DELETE_PERCENTAGE']))
        return int(self.config['INSERT_PERCENTAGE']) / gcd, int(self.config['READ_PERCENTAGE']) / gcd, int(
            self.config['DELETE_PERCENTAGE']) / gcd

    def recvall(self,sock):
        BUFF_SIZE = 4096  # 4 KiB
        data = b''
        while True:
            part = sock.recv(BUFF_SIZE)
            data += part
            if len(part) < BUFF_SIZE:
                # either 0 or end of data
                break
        return data

    def prefill_data(self):
        # Derive the number of keys to be inserted.
        keys = convert_to_int(self.config['PREFILL_KEYS'])
        while keys > 0:
            key = generate_key()
            value = self.msg
            self.insert(key, value)
            self.keys.append(key)
            keys = keys - 1

    def insert(self, key, value):
        data = "*3" + self.CRLF + "$3" + self.CRLF + "SET" + self.CRLF + "$" + str(len(
            key)) + self.CRLF + key + self.CRLF + "$" + str(len(value)) + self.CRLF + value + self.CRLF
        try:
            self.socket_fd.sendall(bytes(data, encoding='utf8'))
            output = self.socket_fd.recv(1024).decode("utf-8")
            #print ("Insert command response " + output)
            if output != "+OK\r\n":
                print("Insert command failed. Response=" + output)
                return -1
        except OSError:
            print('Error occurred while sending INSERT command. Reconnecting to server..')
            self.connect_to_server(True)
            return -1
        return 0

    def read(self, key):
        data = data = "*2" + self.CRLF + "$3" + self.CRLF + "GET" + self.CRLF + "$" + str(len(
            key)) + self.CRLF + key + self.CRLF
        try:
            self.socket_fd.sendall(bytes(data, encoding='utf8'))
            output = self.recvall(self.socket_fd).decode("utf-8")
            #print("Read command response " + output)
            if output == "$-1\r\n":
                print("Read command failed. Response=" + output)
                return ""
            return output[output.find("\r\n")+2:]
        except OSError:
            print('Error occurred while sending READ command. Reconnecting to server..')
            self.connect_to_server(True)

    def delete(self, key):
        data = data = "*2" + self.CRLF + "$3" + self.CRLF + "DEL" + self.CRLF + "$" + str(len(
            key)) + self.CRLF + key + self.CRLF
        try:
            self.socket_fd.sendall(bytes(data, encoding='utf8'))
            output = self.socket_fd.recv(1024).decode("utf-8")
            #print ("Delete command response " + output)
            if output != ":1\r\n":
                print("Delete command failed. Response=" + output)
                return -1;
        except OSError:
            print('Error occurred while sending DELETE command. Reconnecting to server..')
            self.connect_to_server(True)
            return -1;
        return 0

    def start(self):
        insert_count_org, read_count_org, delete_count_org = self.calculate_workload_count()

        #print(insert_count_org, read_count_org, delete_count_org)

        insert_count = insert_count_org
        read_count = read_count_org
        delete_count = delete_count_org

        if 'PREFILL_KEYS' in self.config:
            print('Prefilling data..')
            self.prefill_data()
        value = True
        while value:
            while insert_count > 0:
                key = generate_key()
                #print(' Inserting Key ' + key)
                self.insert(key, self.msg)
                self.keys.append(key)
                insert_count -= 1
            while read_count > 0:
                key = self.keys[int(read_count_org - read_count)]
                #print('Reading Key ' + key)
                self.read(key)
                read_count -= 1
            while delete_count > 0:
                key = self.keys[int(delete_count_org - delete_count)]
                #print('Deleting Key ' + key)
                self.delete(key)
                self.keys.remove(key)
                delete_count -= 1
            insert_count = insert_count_org
            read_count = read_count_org
            delete_count = delete_count_org


if __name__ == '__main__':
    main()
