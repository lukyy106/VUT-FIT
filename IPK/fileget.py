from socket import *
import sys
import os



if len(sys.argv) != 5:
    sys.stderr.write("Wrong number of arguments!\n")
    exit(1)

if sys.argv[1] == "-f" and sys.argv[3] == "-n":
    tmp = sys.argv[2]
    sys.argv[2] = sys.argv[4]
    sys.argv[4] = tmp

    tmp = sys.argv[1]
    sys.argv[1] = sys.argv[3]
    sys.argv[3] = tmp




if sys.argv[1] == "-n" and sys.argv[3] == "-f":
    if sys.argv[4][0] != 'f' or sys.argv[4][1] != 's' or sys.argv[4][2] != 'p' or sys.argv[4][3] != ':' or sys.argv[4][4] != '/' or sys.argv[4][5] != '/':
        sys.stderr.write("Wrong argument \"" + sys.argv[4] + "\" !\n")
        exit(1)

    client_socket = socket(AF_INET, SOCK_DGRAM)
    server_name = sys.argv[4][6:]
    try:
        slash_position = server_name.index('/')
    except:
        sys.stderr.write("Wrong argument \"" + sys.argv[4] + "\" !\n")
        exit(1)
    server_name = server_name[:slash_position]
    try:
        colon_position = sys.argv[2].index(':')
    except:
        sys.stderr.write("Missing : in \"" + sys.argv[2] + "\" !\n")
        exit(1)
    ip_addr = sys.argv[2][:colon_position]
    port = sys.argv[2][colon_position+1:]
    client_socket.settimeout(30)
    try:
        client_socket.sendto(("WHEREIS " + server_name).encode('utf-8'), (ip_addr, int(port)))
    except:
        sys.stderr.write("Faild to send!\n")
        exit(1)
    try:
        recieved_msg = client_socket.recv(1024)
    except:
        sys.stderr.write("Time out!\n")
        exit(1)
    #recieved_msg = recieved_msg.decode('utf-8')
    if recieved_msg[0] == 69:
        sys.stderr.write("Wrong server name!\n")
        exit(1)

    new_server_port = recieved_msg[3+colon_position+1:]
    client_socket = socket(AF_INET, SOCK_STREAM)
    client_socket.connect((ip_addr, int(new_server_port)))
    file_name = sys.argv[4][slash_position+1+6:]
    if file_name == '*':
        file_name = "index"
    client_socket.send(('GET ' + file_name + " FSP/1.0\r\n" + 'Hostname: ' + server_name + '\r\nAgent: xneupa01\r\n\r\n').encode('utf-8'))
    try:
        recieved_msg = client_socket.recv(1024)
    except:
        sys.stderr.write("Time out!\n")
        exit(1)
    if recieved_msg[8] == 78: #78 in ascii N
        sys.stderr.write("File not found\n")
        exit(1)
    while 1:
        try:
            file_slash_position = rest_of_filne_name.index('/')
        except:
            break
        directory_name = current_file[:file_slash_position+len(directory_name)+1]
        rest_of_filne_name = rest_of_filne_name[file_slash_position+1:]
        try:
            os.mkdir(directory_name)
        except:
            pass
    f = open(file_name, "wb")
    once = 1
    while recieved_msg != b'':
        if once:
            nl_position = recieved_msg.index(('\n').encode('utf-8'))
            recieved_msg = recieved_msg[nl_position+1:]
            nl_position = recieved_msg.index(('\n').encode('utf-8'))
            recieved_msg = recieved_msg[nl_position+1:]
            nl_position = recieved_msg.index(('\n').encode('utf-8'))
            recieved_msg = recieved_msg[nl_position+1:]
            once = 0
        f.write(recieved_msg)
        recieved_msg = client_socket.recv(1024)
    f.close()
    if file_name == "index":
        f = open(file_name, "r")
        files = f.readlines()
        for current_file in files:
            rest_of_filne_name = current_file
            directory_name = ''
            while 1:
                try:
                    file_slash_position = rest_of_filne_name.index('/')
                except:
                    break
                directory_name = current_file[:file_slash_position+len(directory_name)+1]
                rest_of_filne_name = rest_of_filne_name[file_slash_position+1:]
                try:
                    os.mkdir(directory_name)
                except:
                    pass
            current_file = current_file.replace('\n', '')
            current_file = current_file.replace('\r', '')
            client_socket = socket(AF_INET, SOCK_STREAM)
            client_socket.connect((ip_addr, int(new_server_port)))
            client_socket.send(('GET ' + current_file + " FSP/1.0\r\n" + 'Hostname: ' + server_name + '\r\nAgent: xneupa01\r\n\r\n').encode('utf-8'))
            try:
                recieved_msg = client_socket.recv(1024)
            except:
                sys.stderr.write("Time out!\n")
                exit(1)
            if recieved_msg[8] == 78 or recieved_msg[8] == 66: #78 in ascii N and 66 B
                sys.stderr.write("File not found\n")
                exit(1)
            new_file = open(current_file, "wb")
            once = 1
            while recieved_msg != b'':
                if once:
                    nl_position = recieved_msg.index(('\n').encode('utf-8'))
                    recieved_msg = recieved_msg[nl_position+1:]
                    nl_position = recieved_msg.index(('\n').encode('utf-8'))
                    recieved_msg = recieved_msg[nl_position+1:]
                    nl_position = recieved_msg.index(('\n').encode('utf-8'))
                    recieved_msg = recieved_msg[nl_position+1:]
                    once = 0
                new_file.write(recieved_msg)
                recieved_msg = client_socket.recv(1024)
    f.close()
    client_socket.close()
else:
    sys.stderr.write("Wrong arguments!\n")
    exit(1)
