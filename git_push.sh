#!/bin/bash

# =================配置区域=================
FILES_PER_COMMIT=100      # 每个提交(Commit)包含的文件数
MAX_RETRY=10              # 提交失败后的最大重试次数
SLEEP_SEC=0.1            # 失败重试的等待时间（优化：减少等待时间）
# ==========================================

# 获取当前分支（优化：只获取一次）
CURRENT_BRANCH=$(git symbolic-ref --short HEAD 2>/dev/null || git rev-parse --abbrev-ref HEAD)
echo "当前分支: $CURRENT_BRANCH"
echo "--- 开始分批处理文件 (每批 $FILES_PER_COMMIT 个文件，提交后立即推送) ---"

batch_num=1
while true; do
    # 获取待处理的文件列表
    FILES=$(git ls-files --others --modified --exclude-standard | head -n "$FILES_PER_COMMIT")
    
    # 检查是否还有文件需要处理
    if [ -z "$FILES" ]; then
        echo "✅ 所有文件已处理完成！"
        break
    fi
    
    # 统计文件数（优化：直接计算）
    FILE_COUNT=$(echo "$FILES" | grep -c . || echo "0")
    if [ "$FILE_COUNT" -eq 0 ]; then
        echo "✅ 所有文件已处理完成！"
        break
    fi
    
    echo "--- 批次 $batch_num: 处理 $FILE_COUNT 个文件 ---"
    
    # 将文件添加到数组（处理文件路径中的空格和特殊字符）
    batch_array=()
    while IFS= read -r file; do
        if [ -n "$file" ]; then
            batch_array+=("$file")
        fi
    done <<< "$FILES"
    
    if [ ${#batch_array[@]} -eq 0 ]; then
        echo "没有更多文件需要处理。"
        break
    fi
    
    # 步骤1: 尝试提交（使用数组方式，更可靠）
    commit_success=false
    commit_attempt=1
    while [ $commit_attempt -le $MAX_RETRY ]; do
        # 使用数组方式添加文件，确保正确处理文件路径
        if git add "${batch_array[@]}" && \
           git commit -m "Batch $batch_num: ${#batch_array[@]} files - $(date '+%H:%M:%S')"; then
            echo "✅ 批次 $batch_num 提交成功 (${#batch_array[@]} 个文件)"
            commit_success=true
            break
        else
            if [ $commit_attempt -lt $MAX_RETRY ]; then
                echo "⚠ 批次 $batch_num 提交失败 (第 $commit_attempt/$MAX_RETRY 次)，重试..."
                sleep $SLEEP_SEC
            fi
            ((commit_attempt++))
        fi
    done
    
    if [ "$commit_success" = false ]; then
        echo "❌ 批次 $batch_num 提交失败，达到最大重试次数。"
        exit 1
    fi
    
    # 步骤2: 立即推送（优化：使用 HEAD，避免额外的 git rev-parse 调用）
    push_success=false
    push_attempt=1
    while [ $push_attempt -le $MAX_RETRY ]; do
        # 优化：直接推送 HEAD，更快
        if git push origin HEAD:"$CURRENT_BRANCH"; then
            echo "✅ 批次 $batch_num 推送成功"
            push_success=true
            break
        else
            if [ $push_attempt -lt $MAX_RETRY ]; then
                echo "⚠ 批次 $batch_num 推送失败 (第 $push_attempt/$MAX_RETRY 次)，重试..."
                sleep $SLEEP_SEC
            fi
            ((push_attempt++))
        fi
    done
    
    if [ "$push_success" = false ]; then
        echo "❌ 批次 $batch_num 推送失败，达到最大重试次数。"
        exit 1
    fi
    
    ((batch_num++))
done

echo "=========================================="
echo "🎉 本次任务完成！"
echo "💡 如果还有未处理的文件，请再次运行脚本。"
