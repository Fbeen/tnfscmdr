#include <stdio.h>
#include <string.h>

#include "types.h"
#include "utils.h"
#include "packet.h"
#include "arp.h"
#include "udp.h"
#include "dns.h"
#include "timer.h"

#include "netw.h"
#include "log.h"

#define NETW_LOCAL_PORT  16384
#define NETW_TIMEOUT_MS  3000
#define NETW_PAYLOAD_OFFSET 42

static IpAddr_t server_ip;
static uint16_t remote_port = 16384;
static uint16_t local_port  = NETW_LOCAL_PORT;
static int      stack_initialized = 0;

static volatile uint8_t packet_received = 0;
static int recv_length = 0;

#define NETW_MAX_PACKET 1500

static uint8_t send_buffer[NETW_MAX_PACKET];
static uint8_t recv_buffer[NETW_MAX_PACKET];

static void netw_udp_callback(const unsigned char *packet, const UdpHeader *udp)
{
    int len;

    len = ntohs(udp->len) - sizeof(UdpHeader);

    if (len < 0) {
        len = 0;
    }

    if (len > NETW_MAX_PACKET) {
        len = NETW_MAX_PACKET;
    }

    memcpy(recv_buffer, packet + NETW_PAYLOAD_OFFSET, len);

    recv_length = len;
    packet_received = 1;

    Buffer_free(packet);
}

static void netw_poll(void)
{
    PACKET_PROCESS_SINGLE;
    Arp::driveArp();
}

static void netw_wait_for_dns(void)
{
    while (Dns::isQueryPending()) {
        netw_poll();
        Dns::drivePendingQuery();
    }
}

void setTimeoutTime(int t)
{
    (void)t;
}

bool netw_isValidIpAddress(char* ipAddress)
{
    (void)ipAddress;
    return false;
}

bool netw_getIpAddress(char* ip, char* hostname)
{
    int rc;

    rc = Dns::resolve(hostname, server_ip, 1);
    netw_wait_for_dns();

    rc = Dns::resolve(hostname, server_ip, 0);

    if (rc != 0) {
        return false;
    }

    sprintf(ip, "%d.%d.%d.%d",
        server_ip[0],
        server_ip[1],
        server_ip[2],
        server_ip[3]);

    return true;
}

void netw_connect(char* host, int port, bool useTCP)
{
    char ip[32];

    (void)useTCP;

    remote_port = (uint16_t)port;
    local_port  = NETW_LOCAL_PORT;

    if (!stack_initialized) {
        if (Utils::parseEnv() != 0) {
            log_printf("Unable to parse mTCP config\n");
            return;
        }

        if (Utils::initStack(0, 0, NULL, NULL)) {
            log_printf("Unable to init TCP/IP stack\n");
            return;
        }

        Udp::registerCallback(local_port, netw_udp_callback);
        stack_initialized = 1;
    }

    log_printf("Resolving %s...\n", host);

    if (!netw_getIpAddress(ip, host)) {
        log_printf("DNS failed\n");
        return;
    }

    log_printf("Server IP: %s port: %u\n", ip, remote_port);
}

void netw_disconnect()
{
}

void netw_shutdown()
{
    if (stack_initialized) {
        Utils::endStack();
        stack_initialized = 0;
    }
}

void netw_send(const uint8_t* buffer, int length)
{
    int rc;
    clockTicks_t start;

    if (length + sizeof(UdpPacket_t) > NETW_MAX_PACKET) {
        log_printf("Packet too large\n");
        return;
    }

    memset(send_buffer, 0, sizeof(send_buffer));
    memcpy(send_buffer + sizeof(UdpPacket_t), buffer, length);

    rc = Udp::sendUdp(
        server_ip,
        local_port,
        remote_port,
        length,
        send_buffer,
        1
    );

    start = TIMER_GET_CURRENT();

    while (rc == 1) {
        netw_poll();

        if (Timer_diff(start, TIMER_GET_CURRENT()) >
            TIMER_MS_TO_TICKS(NETW_TIMEOUT_MS)) {
            log_printf("ARP timeout\n");
            return;
        }

        rc = Udp::sendUdp(
            server_ip,
            local_port,
            remote_port,
            length,
            send_buffer,
            1
        );
    }
}

int netw_recv(uint8_t* buffer, int buffer_size)
{
    clockTicks_t start;

    start = TIMER_GET_CURRENT();

    while (!packet_received) {
        netw_poll();

        if (Timer_diff(start, TIMER_GET_CURRENT()) >
            TIMER_MS_TO_TICKS(NETW_TIMEOUT_MS)) {
            return NETW_ERR_TIMEOUT;
        }
    }

    if (recv_length > buffer_size) {
        recv_length = buffer_size;
    }

    {
        int len;

        len = recv_length;

        memcpy(buffer, recv_buffer, len);

        packet_received = 0;
        recv_length = 0;

        return len;
    }
}