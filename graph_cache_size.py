import matplotlib.pyplot as plt

# 数据
memory_size_kb = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536]

# 不同 STRIDE 的访问时间数据
access_time_256 = [472.536, 518.385, 517.365, 518.989, 548.562, 582.091, 1059.95, 1090.94, 1050.49, 1025.85, 
                   1114.89, 1291.06, 1323.19, 1356.8, 3011.75, 5375.39, 6890.9]
access_time_128 = [479.211, 521.166, 470.124, 494.407, 534.724, 491.23, 554.097, 580.163, 575.388, 581.074, 
                   648.763, 945.24, 919.569, 979.803, 2856.59, 5194.67, 6565.94]
access_time_64 = [451.266, 596.405, 475.777, 492.154, 488.219, 539.443, 592.489, 562.021, 577.68, 634.151, 
                  704.018, 721.296, 702.125, 706.675, 1725.01, 2518.09, 2684.44]
access_time_32 = [465.708, 504.266, 537.263, 517.014, 455.99, 470.586, 548.742, 574.188, 555.177, 584.162, 
                  604.294, 575.455, 699.236, 606.1, 1318.48, 1842.75, 1644.52]
access_time_512 = [375.867, 392.607, 429.578, 429.106, 433.028, 393.593, 980.882, 975.62, 956.576, 967.681, 
                   1051.74, 1218.94, 1348.47, 1869.37, 4651.44, 8097.2, 9433.08]

# 创建折线图
plt.figure(figsize=(12, 6))  # 增大图形宽度以适应更多刻度

# 绘制五组折线，使用不同颜色
plt.plot(memory_size_kb, access_time_256, marker='o', linestyle='-', color='b', label='STRIDE=256')
plt.plot(memory_size_kb, access_time_128, marker='o', linestyle='-', color='orange', label='STRIDE=128')
plt.plot(memory_size_kb, access_time_64, marker='o', linestyle='-', color='green', label='STRIDE=64')
plt.plot(memory_size_kb, access_time_32, marker='o', linestyle='-', color='red', label='STRIDE=32')
plt.plot(memory_size_kb, access_time_512, marker='o', linestyle='-', color='cyan', label='STRIDE=512')

# 设置横坐标为对数刻度并指定刻度
plt.xscale('log')
plt.xticks([1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192, 16384, 32768, 65536], 
           ['1', '2', '4', '8', '16', '32', '64', '128', '256', '512', '1024', '2048', '4096', '8192', '16384', '32768', '65536'],
           rotation=45)  # rotation=45 使刻度标签倾斜，便于阅读

plt.xlabel('Memory Size (KB)')
plt.ylabel('Access Time (ns)')  # 假设单位是纳秒，可根据实际调整
plt.title('Memory Size vs Access Time for Different Strides')

# 添加网格线
plt.grid(True, which="both", ls="--")

# 添加三条垂直虚线
plt.axvline(x=32, color='purple', linestyle='--', label='32 KB')
plt.axvline(x=1024, color='red', linestyle='--', label='1024 KB')
plt.axvline(x=16384, color='brown', linestyle='--', label='16*1024 KB')

# 添加图例
plt.legend()

# 调整布局以防止刻度标签被裁切
plt.tight_layout()

# 保存图像
plt.savefig('memory_access_time_multi_strides_with_new.png', dpi=300, bbox_inches='tight')
print("The graph has been saved as 'memory_access_time_multi_strides_with_new.png'")

# 显示图像（可选）
plt.show()