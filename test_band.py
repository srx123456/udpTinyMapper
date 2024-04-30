import csv
import subprocess

# 创建一个CSV文件并写入表头
with open('iperf_results.csv', mode='w', newline='') as file:
    writer = csv.writer(file)
    writer.writerow(['Bandwidth (Mbits/sec)', 'Packet Loss Rate (%)'])

    # 循环测试不同带宽
    for band in range(251):
        # 运行iperf命令
        command = f'iperf -c 192.168.1.2 -p 4853 -u -b {band}M'
        result = subprocess.run(command, shell=True, text=True, capture_output=True)

        # 解析iperf的输出，提取丢包率
        output_lines = result.stdout.split('\n')
        packet_loss_rate = None
        for line in output_lines:
            if '0.0-10.0' in line:
                parts = line.split()
                packet_loss_rate = float(parts[-2])  # 修改这里获取丢包率的方式
                break

        # 将结果写入CSV文件
        writer.writerow([band, packet_loss_rate])
