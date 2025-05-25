import matplotlib.pyplot as plt

# 数据
stride_size = [2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]
access_time = [635.378, 625.461, 639.181, 722.808, 709.042, 673.514, 652.751, 682.635, 630.265, 689.421, 642.706]

# 创建折线图
plt.figure(figsize=(10, 6))  # 设置图形大小
plt.plot(stride_size, access_time, marker='o', linestyle='-', color='b', label='Access Time')

# 设置横坐标并指定刻度
plt.xticks([2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12], 
           ['2', '3', '4', '5', '6', '7', '8', '9', '10', '11', '12'])

plt.xlabel('Stride Size')
plt.ylabel('Access Time (ns)')  # 假设单位是纳秒，可根据实际调整
plt.title('Stride Size vs Access Time')

# 添加网格线
plt.grid(True, which="both", ls="--")

# 添加垂直虚线
plt.axvline(x=5, color='r', linestyle='--', label='Stride=5')

# 添加图例
plt.legend()

# 调整布局以防止刻度标签被裁切
plt.tight_layout()

# 保存图像
plt.savefig('stride_access_time_plot_with_line_at_5.png', dpi=300, bbox_inches='tight')
print("The graph has been saved as 'stride_access_time_plot_with_line_at_5.png'")

# 显示图像（可选）
plt.show()