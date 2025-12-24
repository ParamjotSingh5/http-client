#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>

int main(){
    struct addrinfo hints = {0}; // Sets everything to zero immediately
    struct addrinfo *resolvedAddressInfoPtr; // The getaddrinfo will prepare a LinkedList of type addrinfo in heap. 
    // then it needs to tell us the memory address of the first item in that list.
    // To change the value of the pointer resolvedAddressInfoPtr so that it points to the new memory location, the function needs the address of your pointer.
    
    int status;
    char ipstr[INET6_ADDRSTRLEN];
    char *host = "www.google.com";

    // Define attributes values to filter conections we are willing to support and connect. The getaddrinfo function will use these values while resolving available addresses on host.
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM; // We  TCP contract 

    if ((status = getaddrinfo(host, "80", &hints, &resolvedAddressInfoPtr)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
        return 1;
    }

    printf("IP addresses for www.google.com:\n\n");
    for(addrinfo *itr = resolvedAddressInfoPtr; itr != NULL; itr = itr -> ai_next){
        void *addr;
        char *ipver;

        if(itr->ai_family == AF_INET){
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)itr->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = "IPv4";
        }
        else{
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)itr->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver= "IPv6";
        }

        inet_ntop(itr->ai_family, addr, ipstr, sizeof ipstr); // Translate IPAddress Network (32-bit binary) to Presentation (192.168.1.1).
        printf(" %s: %s\n", ipver, ipstr);
    }

    freeaddrinfo(resolvedAddressInfoPtr);

    return 0;
}