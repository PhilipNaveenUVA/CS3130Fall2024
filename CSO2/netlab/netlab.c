#include <stdio.h>
#include <stdbool.h>
#include "netsim.h"

void resend_get_request(void *arg);
void send_ack(char seq_number);
char calculate_checksum(char *data, size_t len);

static bool ack_received = false;  // Track if acknowledgment has been received

// Function to calculate checksum (XOR of all bytes except the first one)
char calculate_checksum(char *data, size_t len) {
    char checksum = 0;
    for (size_t i = 1; i < len; i++) {
        checksum ^= data[i];
    }
    return checksum;
}

// Function to send an acknowledgment message
void send_ack(char seq_number) {
    char ack_data[4] = {0, 'A', 'C', 'K'};
    ack_data[3] = seq_number;  // Add sequence number
    ack_data[0] = calculate_checksum(ack_data, 4);  // Calculate checksum
    send(4, ack_data);  // Send ACK message
}

// Function to handle received messages
void recvd(size_t len, void* _data) {
    char *data = _data;
    char received_checksum = data[0];
    char calculated_checksum = calculate_checksum(data, len);

    printf("Received message of length %zu: ", len);
    for (size_t i = 0; i < len; i++) {
        printf("%02x ", (unsigned char)data[i]);
    }
    printf("\n");

    if (received_checksum == calculated_checksum) {
        // If checksum matches, print the received data starting from the 4th byte
        fwrite(data + 3, 1, len - 3, stdout);
        fflush(stdout);

        // Send acknowledgment with the sequence number
        send_ack(data[1]);
        
        // Mark acknowledgment as received to prevent further retransmissions
        ack_received = true;
    } else {
        printf("Checksum mismatch, ignoring message\n");
    }
}

// Function to resend the GET request if no acknowledgment is received
void resend_get_request(void *arg) {
    if (!ack_received) {  // Only resend if acknowledgment was not received
        char data[5];
        data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = *(char *)arg;
        data[0] = calculate_checksum(data, 5);
        send(5, data);
        
        // Set timeout for another retry if still no response
        setTimeout(resend_get_request, 1000, arg);
    }
}

// Main function
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "USAGE: %s n\n    where n is a number between 0 and 3\n", argv[0]);
        return 1;
    }

    // Prepare GET message
    char data[5];
    data[1] = 'G'; data[2] = 'E'; data[3] = 'T'; data[4] = argv[1][0];
    data[0] = calculate_checksum(data, 5);

    // Send initial GET request
    send(5, data);

    // Set timeout to check if the first message is acknowledged
    setTimeout(resend_get_request, 1000, argv[1]);

    // Wait for all messages and timeouts
    waitForAllTimeoutsAndMessagesThenExit();

    return 0;
}

