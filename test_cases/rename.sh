#!/bin/bash

# 获取当前文件夹路径
current_dir=$(pwd)

echo "char* test_cases[]={"

# 遍历当前文件夹下的所有文件
for file in "$current_dir"/*.c; do
    # 判断是否为文件
    if [ -f "$file" ]; then
        # 提取文件名
        filename=$(basename "$file")
        # 输出文件名加前缀和后缀
        echo \"./test_cases/$filename\",
    fi
done
echo }

