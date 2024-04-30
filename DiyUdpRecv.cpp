#include <iostream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

uint32_t start_maker = 0xaabbccdd;
uint32_t end_maker = 0xccddeeff;
uint32_t full_maker = 0xffffffff;

int main() {
    const int PORT = 7676;
    const int MAX_BUFFER_SIZE = 1024;

    // 创建UDP套接字
    int udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_sock < 0) {
        perror("Error creating socket");
        return 1;
    }

    // 绑定套接字到本地端口
    struct sockaddr_in local_address;
    memset(&local_address, 0, sizeof(local_address));
    local_address.sin_family = AF_INET;
    local_address.sin_port = htons(PORT);
    local_address.sin_addr.s_addr = htonl(INADDR_ANY);
    if (bind(udp_sock, (struct sockaddr *)&local_address, sizeof(local_address)) < 0) {
        perror("Error binding socket");
        return 1;
    }

    // 接收数据包并存放其源地址
    struct sockaddr_in sender_address;
    socklen_t sender_address_len = sizeof(sender_address);
    char buffer[MAX_BUFFER_SIZE];
    memset(buffer, 0, sizeof(buffer));

    while (true) {
        int recv_len = recvfrom(udp_sock, buffer, MAX_BUFFER_SIZE, 0, (struct sockaddr *)&sender_address, &sender_address_len);
        if (recv_len < 0) {
            perror("Error receiving data");
            break;
        }

        // 将源地址转换成字符串形式
        char sender_ip[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &(sender_address.sin_addr), sender_ip, INET_ADDRSTRLEN);

        //将buffer中隐藏的地址逐步释放解析出来
        char* found_ptr = buffer;
        while (found_ptr + sizeof(uint32_t) < buffer + sizeof(buffer)) {
            // 找到了起始标志符
            if (std::memcmp(found_ptr, &start_maker, sizeof(start_maker)) == 0 && std::memcmp(found_ptr + sizeof(start_maker), &end_maker, sizeof(end_maker)) != 0) {
                // 读取后续16字节的内容
                char next_hop_ip[16];
                // further_address
                std::memcpy(next_hop_ip, found_ptr + sizeof(start_maker), sizeof(next_hop_ip));
                // 将char数组的地址转换为sockaddr_in结构体的地址
                sockaddr_in* further_address = reinterpret_cast<sockaddr_in*>(next_hop_ip);

                // 打印读取的内容
                char ip[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &(further_address->sin_addr), ip, INET_ADDRSTRLEN);
                std::cout << "IP: " << ip << ", Port: " << ntohs(further_address->sin_port) << std::endl;

                // 将start_maker标志位右移16字节
                // 并将标志位前的16个字节赋4个值full_maker
                std::memcpy(found_ptr, &full_maker, (4 * sizeof(full_maker)));
                std::memcpy(found_ptr+16, &start_maker, sizeof(start_maker));

                break;  // 停止查找
            }
            // 到达最后一跳SOR，将所有的填充都删除，恢复原貌
            else if(std::memcmp(found_ptr + sizeof(start_maker), &end_maker, sizeof(end_maker)) == 0){
                char* del_ptr = buffer;
                int del_len = 0;
                std::cout << "* i am the last! " << std::endl;
                std::cout << "* the data's length is  " << strlen(buffer) << " bytes!(undeleted)" << std::endl;
                while (del_ptr + sizeof(uint32_t) <= found_ptr + sizeof(start_maker)){
                    // 寻找当前数据中的del_maker
                    if (std::memcmp(del_ptr, &full_maker, sizeof(full_maker)) == 0){
                        del_len = found_ptr + sizeof(start_maker) + sizeof(end_maker) - del_ptr;
                        std::cout << "* the data has been deleted " << del_len << " bytes!" << std::endl;
                        std::memcpy(del_ptr, found_ptr + sizeof(start_maker) + sizeof(end_maker), recv_len - del_len);
                        std::cout << "* the data's length is  " << strlen(buffer) << " bytes!" << std::endl;
                        break;
                    }
                    // 只找到了start_maker，删除start_maker和end_maker，只是为了方便测试
                    else{
                        del_len = sizeof(start_maker) + sizeof(end_maker);
                        std::cout << "* the data has been deleted " << del_len << " bytes!" << std::endl;
                        std::memcpy(found_ptr, found_ptr + sizeof(start_maker) + sizeof(end_maker), recv_len - del_len);
                        std::cout << "* the data's length is  " << strlen(buffer) << " bytes!" << std::endl;
                        break;
                    }
                }
            }
            found_ptr += 1;  // 继续查找下一个可能的起始标志符
        }
        std::cout << "Received from " << sender_ip << ":" << ntohs(sender_address.sin_port) << ", data's length:" << recv_len << std::endl;
    }

    // 关闭套接字
    close(udp_sock);

    return 0;
}
