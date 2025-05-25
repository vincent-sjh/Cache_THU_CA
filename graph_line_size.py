import matplotlib.pyplot as plt

# 数据
stride_size = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024]
access_time = [690.056, 662.309, 671.778, 684.829, 704.903, 771.807, 827.341, 810.809, 813.353, 953.564]

# 创建折线图
plt.figure(figsize=(10, 6))  # 设置图形大小
plt.plot(stride_size, access_time, marker='o', linestyle='-', color='b', label='Access Time')

# 设置横坐标为对数刻度并指定刻度
plt.xscale('log')
plt.xticks([2, 4, 8, 16, 32, 64, 128, 256, 512, 1024], 
           ['2', '4', '8', '16', '32', '64', '128', '256', '512', '1024'],
           rotation=45)  # rotation=45 使刻度标签倾斜，便于阅读

plt.xlabel('Stride Size')
plt.ylabel('Access Time (ns)')  # 假设单位是纳秒，可根据实际调整
plt.title('Stride Size vs Access Time')

# 添加网格线
plt.grid(True, which="both", ls="--")

# 添加垂直虚线
plt.axvline(x=64, color='r', linestyle='--', label='Stride=64')

# 添加图例
plt.legend()

# 调整布局以防止刻度标签被裁切
plt.tight_layout()

# 保存图像
plt.savefig('stride_access_time_plot_with_line.png', dpi=300, bbox_inches='tight')
print("The graph has been saved as 'stride_access_time_plot_with_line.png'")

# 显示图像（可选）
plt.show()