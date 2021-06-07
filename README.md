server : gcc - o server server.c -lpthread  
       : ./server 9090

client : gcc -o client client.c  
       : ./client 10.0.2.15 9090 hansol
