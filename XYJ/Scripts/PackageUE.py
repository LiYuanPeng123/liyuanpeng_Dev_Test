import os
import sys
import subprocess
import datetime
import argparse

def main():
    # 1. 设置基础路径 (Setup paths)
    # 获取当前脚本所在目录
    script_dir = os.path.dirname(os.path.abspath(__file__))
    # 推导 .uproject 文件路径
    project_path = os.path.abspath(os.path.join(script_dir, "..", "XYJ.uproject"))
    
    # 2. 解析命令行参数 (Parse arguments)
    parser = argparse.ArgumentParser(description="Package Unreal Engine Project (XYJ)")
    parser.add_argument("platform", nargs="?", default="Win64", help="Target Platform (default: Win64)")
    parser.add_argument("config", nargs="?", default="DebugGame", help="Build Configuration (default: DebugGame)")
    args = parser.parse_args()
    
    platform = args.platform
    config = args.config
    
    # 3. 查找 UE_ROOT (Find UE_ROOT)
    # 优先检查环境变量，最后回退到默认路径
    ue_root = os.environ.get("UE_ROOT")
    if not ue_root:
        ue_root = os.environ.get("UE5_ROOT")
    if not ue_root:
        # Batch 脚本中的默认路径
        ue_root = r"C:\Program Files\Epic Games\UE_5.6"
        
    # 4. 检查 RunUAT 是否存在 (Check RunUAT)
    uat_script = os.path.join(ue_root, "Engine", "Build", "BatchFiles", "RunUAT.bat")
    if not os.path.exists(uat_script):
        print(f"Error: RunUAT.bat not found at {ue_root}")
        print("Please set UE_ROOT or UE5_ROOT environment variable to your Unreal Engine installation.")
        sys.exit(1)
        
    # 5. 设置输出目录 (Setup Output Directory)
    # 生成时间戳 YYYYMMDD_HHMMSS
    now = datetime.datetime.now()
    timestamp = now.strftime("%Y%m%d_%H%M%S")
    
    # 构建输出路径: ../../Builds/Platform/Config/Timestamp
    output_dir = os.path.abspath(os.path.join(script_dir, "..", "..", "Builds", platform, config, timestamp))
    
    if not os.path.exists(output_dir):
        try:
            os.makedirs(output_dir)
        except OSError as e:
            print(f"Error creating output directory {output_dir}: {e}")
            sys.exit(1)
        
    print("-" * 50)
    print(f"Packaging Project: {project_path}")
    print(f"Platform:          {platform}")
    print(f"Configuration:     {config}")
    print(f"UE Root:           {ue_root}")
    print(f"Output Directory:  {output_dir}")
    print("-" * 50)
    
    # 6. 执行构建命令 (Execute Build Command)
    # 构建 RunUAT 参数
    # BuildCookRun -project="..." -noP4 -platform=... ...
    cmd = [
        uat_script,
        "BuildCookRun",
        f"-project={project_path}",
        "-noP4",
        f"-platform={platform}",
        f"-clientconfig={config}",
        "-cook",
        "-build",
        "-stage",
        "-pak",
        "-archive",
        f"-archivedirectory={output_dir}",
        "-prereqs",
        "-unattended",
        "-utf8output",
        f"-targetplatform={platform}"
    ]
    
    try:
        # 在 Windows 上调用 .bat 文件通常不需要 shell=True，直接调用即可
        # 实时输出子进程的日志
        result = subprocess.run(cmd)
        sys.exit(result.returncode)
    except Exception as e:
        print(f"Failed to execute packaging command: {e}")
        sys.exit(1)

if __name__ == "__main__":
    main()