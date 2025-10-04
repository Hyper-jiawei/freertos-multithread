
# 2025年10月4日

# FreeRTOS 多任务与栈溢出检测

## 任务创建与栈分配

- 本工程共创建了五个任务：
  - `task1` ~ `task4`：共同使用同一个 `PrintTask()` 函数进行屏幕打印。
  - `Default` 任务：默认分配的 stack 大小为 `128 * 4`。
  - 用户自定义的四个任务，每个任务 stack 大小为 `128`。

> **总共分配的栈空间**：
> 
> 128*4（Default） + 128 + 128 + 128 + 128 = 1024 word = 1024 * 4 = 4096 字节 ≈ 4KB

## FreeRTOS 栈溢出检测方法

为防止任务栈溢出导致系统异常，建议开启 FreeRTOS 的栈溢出检测功能。

### 步骤 1：打开栈溢出检测

在 `FreeRTOSConfig.h` 文件中，添加或修改如下宏定义：

```c
#define configCHECK_FOR_STACK_OVERFLOW 2  // 推荐用2，检测更严格
```

### 步骤 2：实现栈溢出钩子函数

在的工程代码（如 `freertos.c` 或 `main.c`）中添加如下函数：

```c
// 可以放在 freertos.c 或 main.c
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    // 这里可以打断点、点灯、打印等
    // 比如点亮一个LED，或者打印任务名
    printf("Stack overflow in task: %s\r\n", pcTaskName);
    while(1); // 死循环，方便调试
}
```

### 说明

- 当有任务发生栈溢出时，程序会进入 `vApplicationStackOverflowHook`，可通过串口、LED 或调试器发现问题。
- `configCHECK_FOR_STACK_OVERFLOW` 设为 2 检查更严格。
- 如果用 printf，确保不会再导致栈溢出（可用 LED 或断点简单测试）。


![图片说明](image/Hyper_jiawei_github.png)
