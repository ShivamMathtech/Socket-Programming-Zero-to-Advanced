#include <arpa/inet.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
/* Step: Copy the representation to bytes without violating alignment or aliasing. */
int main(void) {
    uint16_t host = 9000;
    uint16_t network = htons(host);
    unsigned char bytes[sizeof network];
    memcpy(bytes, &network, sizeof bytes);
    printf("port=%u wire=%02x %02x roundtrip=%u\n",
           (unsigned)host, (unsigned)bytes[0], (unsigned)bytes[1],
           (unsigned)ntohs(network));
    return 0;
}
