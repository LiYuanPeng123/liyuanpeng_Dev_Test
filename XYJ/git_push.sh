#!/bin/bash

BATCH_SIZE=50   # 每批文件数量
PREVIEW=false   # true=只预览，不提交推送
MAX_RETRY=5     # 每批推送最多重试次数
SLEEP_SEC=5     # 重试前等待秒数

# 获取当前分支名称
CURRENT_BRANCH=$(git symbolic-ref --short HEAD 2>/dev/null || git rev-parse --abbrev-ref HEAD)
if [ -z "$CURRENT_BRANCH" ]; then
    echo "❌ 无法获取当前分支名称"
    exit 1
fi
echo "当前分支: $CURRENT_BRANCH"

FILES=$(git ls-files --others --modified --exclude-standard)
TOTAL_FILES=$(echo "$FILES" | wc -l)
echo "检测到待提交文件数量: $TOTAL_FILES"

if [ "$TOTAL_FILES" -eq 0 ]; then
    echo "❌ 没有检测到需要提交的文件"
    exit 0
fi

count=0
batch_num=1
batch=()

push_with_retry() {
    local attempt=1
    while [ $attempt -le $MAX_RETRY ]; do
        echo "尝试推送 (第 $attempt/$MAX_RETRY 次)..."
        if git push origin "$CURRENT_BRANCH"; then
            echo "✅ 批次推送成功"
            return 0
        else
            echo "⚠ 推送失败，等待 $SLEEP_SEC 秒后重试..."
            sleep $SLEEP_SEC
            ((attempt++))
        fi
    done
    echo "❌ 批次推送最终失败"
    exit 1
}

for file in $FILES; do
    batch+=("$file")
    ((count++))

    if [ "$count" -ge "$BATCH_SIZE" ]; then
        echo "=== 批次 #$batch_num (${#batch[@]} 个文件) ==="
        printf '%s\n' "${batch[@]}"
        if [ "$PREVIEW" = false ]; then
            git add "${batch[@]}"
            git commit -m "Batch commit #$batch_num - $(date '+%Y-%m-%d %H:%M:%S')"
            push_with_retry
        fi
        batch=()
        count=0
        ((batch_num++))
    fi
done

if [ "$count" -gt 0 ]; then
    echo "=== 批次 #$batch_num (${#batch[@]} 个文件) ==="
    printf '%s\n' "${batch[@]}"
    if [ "$PREVIEW" = false ]; then
        git add "${batch[@]}"
        git commit -m "Batch commit #$batch_num (remaining) - $(date '+%Y-%m-%d %H:%M:%S')"
        push_with_retry
    fi
fi

echo "✅ 分批推送完成"