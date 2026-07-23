# STM32F103 + DRV8313 + SimpleFOC 混合式步进电机调试记录

## 1. 项目概况

本工程使用：

- MCU：STM32F103
- 驱动器：DRV8313，三个独立半桥
- 控制库：SimpleFOC 2.4.0
- 电机类型：两相混合式步进电机
- 磁编码器：AS5147，SPI 接口
- 初始调试方式：无编码器、开环速度控制
- 当前控制方式：磁编码器闭环速度控制、SVPWM、电压模式

程序入口为 [`drv8313_stepper.ino`](./drv8313_stepper.ino)。

最终验证可用的 PWM 引脚顺序为：

```cpp
BLDCDriver3PWM driver(PB1, PB0, PA8, PB3);
```

对应关系为：

| SimpleFOC 相位 | STM32 引脚 | DRV8313 输出作用 |
|---|---:|---|
| A | PB1 | 绕组 A 的非公共端 |
| B | PB0 | 绕组 B 的非公共端 |
| C | PA8 | 两个绕组的公共驱动端 |
| EN1 | PB3 | 驱动使能，具体连接取决于电路板 |

### 公共端必须是第三个 PWM 参数

`BLDCDriver3PWM` 的构造函数顺序为：

```cpp
BLDCDriver3PWM(phA, phB, phC, enable);
```

对 `HybridStepperMotor` 而言，SimpleFOC 会固定把 `phC` 对应的桥臂作为两个步进电机绕组的中间公共桥臂。因此，连接 `A2 + B2` 公共点的实际 PWM 引脚必须放在构造函数的第三个参数，不能只按引脚编号或接线方便程度任意排列。

本项目中公共桥臂由 PA8 控制，所以正确顺序是：

```cpp
BLDCDriver3PWM driver(PB1, PB0, PA8, PB3);
//                                ^^^
//                       A2 + B2 公共端对应的桥臂
```

前两个参数分别控制两个绕组的非公共端，必要时可通过交换它们改变相位映射或旋转方向；第三个参数必须始终对应物理公共端。若把公共桥臂放在第一或第二个参数，即使某些固定电角度测试能产生保持力，连续旋转磁场仍会计算错误，电机可能左右摆动、抖动或无法起动。

## 2. 最终故障原因

故障由软件配置和电机接线两部分共同造成。

### 2.1 软件没有持续更新相电压

最初程序中 `motor.loopFOC()` 被注释，只调用了：

```cpp
motor.move(target_velocity);
```

在本机安装的 SimpleFOC 2.4.0 中：

- `motor.move()` 更新目标、开环虚拟角度以及电压/电流给定；
- `motor.loopFOC()` 根据电角度计算三路相电压并写入 PWM。

因此必须在循环中同时调用：

```cpp
motor.loopFOC();
motor.move(target_velocity);
```

缺少 `loopFOC()` 时，驱动器停留在初始化后的零差分电压状态。电机可能只有短路阻尼，表现为手动转动时略有阻力，但不会产生旋转磁场。

### 2.2 错误使用了 `foc_current` 模式

最初配置了：

```cpp
motor.torque_controller = TorqueControlType::foc_current;
```

但工程没有初始化和连接电流传感器。在 `foc_current` 模式下，SimpleFOC 检测不到 `current_sense` 会直接退出本次 FOC 计算，不再写入新的相电压。

此外，SimpleFOC 2.4.0 中 `HybridStepperMotor` 当前应优先使用电压模式。正确配置为：

```cpp
motor.torque_controller = TorqueControlType::voltage;
```

也可以不写这一行，因为 `HybridStepperMotor` 默认就是电压模式。

### 2.3 最终根因：两个绕组的公共端接错

修正软件后，电机可以产生保持力，但运行时仍表现为：

- `target_velocity = 0` 时能够保持位置；
- 低速时左右摆动；
- 高速时只抖动，或需要外力推动后才能转动。

最终确认，DRV8313 三半桥拓扑中的绕组公共端连接错误。将两个不同绕组各自的一端，即 `A2 + B2`，连接在一起后接到 OUTC，电机恢复正常运行。

正确接线如下：

```text
OUTA ── A1 ──[ 绕组 A ]── A2 ──+
                                  +── OUTC
OUTB ── B1 ──[ 绕组 B ]── B2 ──+
```

也就是：

```text
OUTA -> A1
OUTB -> B1
OUTC -> A2 + B2
```

这里的 `A2` 和 `B2` 必须来自两个不同绕组。不能把同一个绕组的两根线接在一起作为公共点，否则两个软件相位可能实际作用在同一个磁轴上：静态时看起来都有保持力，连续控制时却只能产生往复磁场，最终表现为左右抖动。

## 3. 完整调试过程

### 阶段一：电机完全不转，手动转动略有阻力

初始现象：

- 电机不主动旋转；
- 手动旋转时能感觉到一点阻力；
- 串口仍打印 `Motor ready.`。

代码检查发现：

1. `motor.loopFOC()` 被注释；
2. 使用了 `foc_current`，但电流传感器全部被注释；
3. `driver.init()` 和 `motor.init()` 的返回值没有检查；
4. `Motor ready.` 无论初始化是否成功都会打印，因此不能作为初始化成功的依据。

修正为电压模式并恢复 `motor.loopFOC()` 后，电机开始能够运动。

### 阶段二：电机能转，但低速抖动

随后观察到：

- 增大 `motor.voltage_limit` 后电机有时能转；
- `target_velocity = 0` 时能够保持；
- `target_velocity = 1` 时左右摆动；
- `target_velocity = 10` 时有时需要外力推动；
- 12 V 电源显示电流约 0.115 A。

这类现象不能简单归因于电压不足。缺相、绕组公共端错误、驱动使能不完整以及开环失同步都可能出现相似表现。继续盲目提高电压可能造成正常绕组过流。

### 阶段三：使用固定电角度分别测试两条绕组支路

使用以下测试，仅给一条绕组施加静态电压：

```cpp
motor.setPhaseVoltage(2.0f, 0, 0);
```

以及：

```cpp
motor.setPhaseVoltage(2.0f, 0, _PI_2);
```

测试时应暂时注释正常循环中的：

```cpp
motor.loopFOC();
motor.move(target_velocity);
```

在 `BLDCDriver3PWM(PA8, PB0, PB1, PB3)` 的旧顺序下：

- 电角度为 0 时有明显保持力；
- 电角度为 `_PI_2` 时只有类似黏滞阻尼的轻微阻力。

这说明软件定义的 A、B、公共 C 与实际硬件输出和电机接线不一致。

将引脚顺序修改为：

```cpp
BLDCDriver3PWM driver(PB1, PB0, PA8, PB3);
```

之后两个固定电角度均能产生保持力，说明：

- PB0 与 PA8 之间可以建立绕组电流；
- PB1 与 PA8 之间也可以建立绕组电流；
- PA8 对应的桥臂适合作为软件定义中的公共 C 相。

但静态保持测试只能证明两条电流路径存在，不能证明这两条路径确实对应两个不同且正交的电机绕组。

### 阶段四：检查绕组配对并修正公共点

断电并将电机与驱动器断开后，使用万用表识别两组独立绕组：

```text
绕组 A：A1 <-> A2
绕组 B：B1 <-> B2
```

两个绕组的直流电阻应比较接近，且任意跨绕组测量都应开路。

最终将 `A2 + B2` 接在一起并连接 OUTC，剩余的 `A1`、`B1` 分别连接 OUTA、OUTB，电机恢复正常。

## 4. 无编码器阶段的最小控制代码

下面代码用于绕组接线和驱动器的初始开环验证。当前接入磁编码器后的闭环配置见第 12 节和实际工程程序。

```cpp
#include <SimpleFOC.h>

HybridStepperMotor motor(50);
BLDCDriver3PWM driver(PB1, PB0, PA8, PB3);

float target_velocity = 0.0f;

void setup() {
  Serial.begin(115200);
  SimpleFOCDebug::enable(&Serial);

  driver.voltage_power_supply = 12.0f;

  if (!driver.init()) {
    Serial.println("Driver init failed!");
    while (true) {
      delay(1000);
    }
  }

  motor.linkDriver(&driver);

  motor.controller = MotionControlType::velocity_openloop;
  motor.torque_controller = TorqueControlType::voltage;
  motor.foc_modulation = FOCModulationType::SpaceVectorPWM;

  // 应根据绕组电阻和额定电流确定，首次测试从低值开始。
  motor.voltage_limit = 2.0f;

  if (!motor.init()) {
    Serial.println("Motor init failed!");
    while (true) {
      delay(1000);
    }
  }

  Serial.println("Motor ready.");
}

void loop() {
  motor.loopFOC();
  motor.move(target_velocity);
}
```

在无编码器的 `velocity_openloop` 模式下，不要求执行 `motor.initFOC()`。以后接入编码器并切换到闭环控制时，才需要进行传感器链接、方向检测和电角度对齐。

## 5. 推荐的分相测试方法

### 5.1 固定 B 相测试

在当前引脚顺序下：

```cpp
void loop() {
  motor.setPhaseVoltage(2.0f, 0, 0);
}
```

主要测试 OUTB—OUTC，即 PB0 对应桥臂、PA8 对应公共桥臂和绕组 B。

### 5.2 固定 A 相测试

```cpp
void loop() {
  motor.setPhaseVoltage(2.0f, 0, _PI_2);
}
```

主要测试 OUTA—OUTC，即 PB1 对应桥臂、PA8 对应公共桥臂和绕组 A。

两种状态应具有相近的保持力。若差异很大，应检查：

- 两个绕组的直流电阻；
- PWM 引脚到 DRV8313 IN 引脚的连接；
- EN1、EN2、EN3 是否全部有效；
- DRV8313 的 `nFAULT`；
- OUTA、OUTB、OUTC 到电机的连接；
- 某个桥臂是否损坏或进入保护。

### 5.3 离散旋转磁场测试

两个固定角度都有保持力后，可用下列程序检查磁场是否连续地沿同一方向旋转：

```cpp
float electrical_angle = 0.0f;

void loop() {
  motor.setPhaseVoltage(2.0f, 0, electrical_angle);
  delay(500);

  electrical_angle += _PI_2;
  if (electrical_angle >= _2PI) {
    electrical_angle -= _2PI;
  }
}
```

常见 1.8° 步进电机应每次稳定地向同一方向走一个整步。如果只在两个位置之间来回摆动，应优先重新检查绕组配对和公共端连接。

## 6. 电压和电流注意事项

### 6.1 `voltage_power_supply` 与 `voltage_limit` 含义不同

```cpp
driver.voltage_power_supply = 12.0f;
```

必须填写驱动器实际母线电压，它参与 PWM 占空比换算。

```cpp
motor.voltage_limit = 2.0f;
```

表示控制器允许施加到电机旋转电压矢量上的上限，不是电机相电流上限。

在低速或静止时，可以粗略估算：

```text
相电流峰值约为：Iphase ~= voltage_limit / Rphase
```

其中 `Rphase` 是一个完整绕组的直流电阻。例如绕组电阻为 2 Ω，`voltage_limit = 5 V` 时，静态相电流可能接近 2.5 A，存在电机和驱动器过流风险。

首次调试建议：

1. 从 1～2 V 开始；
2. 测量绕组电阻并查询电机额定电流；
3. 逐步增加电压，同时观察相电流、DRV8313 温度和 `nFAULT`；
4. 不要因为电机抖动就直接提高电压，先排除接线、缺相和开环失同步。

### 6.2 可调电源显示电流不等于绕组相电流

DRV8313 通过 PWM 从 12 V 母线向绕组供电，绕组电流还会在桥臂中续流。因此电源显示的是母线平均电流，不能直接作为绕组峰值或 RMS 电流。

同时应确认：

- 电源处于 CV 恒压状态，而不是 CC 限流状态；
- 电机运行时驱动板 VM 端仍接近 12 V；
- 电源线、接插件和地线没有明显压降。

## 7. 开环控制注意事项

### 7.1 速度指令不是加速度限制

在 `velocity_openloop` 模式下，直接把目标从 0 改成 10 rad/s，会让旋转磁场几乎立即达到目标电频率。转子可能无法拉入同步，只会抖动，或者需要外力推动后才开始旋转。

对于 50 极对电机：

```text
10 rad/s 约等于 95.5 rpm 机械转速
电频率约为 10 * 50 / (2*pi) = 79.6 Hz
```

建议：

- 上电后先以 `target_velocity = 0` 静态保持一段时间；
- 对速度指令增加加速度斜坡；
- 从 0.1～0.5 rad/s 开始验证；
- 空载验证后再逐步增加速度和机械负载。

`motor.velocity_limit` 不会自动为 `velocity_openloop` 生成速度斜坡，`PID_velocity` 参数在该开环模式下也不会解决失同步问题。

### 7.2 极对数检查

常见两相混合式步进电机可粗略按下式确定极对数：

```text
极对数 = 整步数/圈 / 4
```

例如：

- 1.8°、200 整步/圈：通常为 50 极对；
- 0.9°、400 整步/圈：通常为 100 极对。

极对数错误会导致设定机械速度与实际速度不一致，也可能增加开环失同步概率。

## 8. DRV8313 使能和 STM32F103 引脚注意事项

### 8.1 第四个构造参数只表示第一个使能引脚

```cpp
BLDCDriver3PWM driver(PB1, PB0, PA8, PB3);
```

第四个参数对应 `en1`，不会在软件中自动复制为 EN2、EN3。DRV8313 有三个独立使能输入，必须确保：

- 三个 EN 在硬件上已经连接在一起，由 PB3 统一控制；或
- 未由 MCU 控制的 EN 已被正确拉高；或
- 分别给构造函数传入三个使能引脚。

如果使用三个独立 MCU 引脚，应采用：

```cpp
BLDCDriver3PWM driver(PB1, PB0, PA8, EN1_PIN, EN2_PIN, EN3_PIN);
```

### 8.2 PB3 与 JTAG 复用

STM32F103 的 PB3 默认复用为 JTDO/SWO。如果实际测得 PB3 无法拉高，可能需要关闭 JTAG、保留 SWD：

```cpp
__HAL_RCC_AFIO_CLK_ENABLE();
__HAL_AFIO_REMAP_SWJ_NOJTAG();
```

这段代码应放在 `driver.init()` 之前。也可以换用不与调试接口复用的普通 GPIO。是否需要该设置应以 EN 引脚实测电平为准。

## 9. 测量安全

- 测量绕组电阻前必须断电，并将电机与驱动器断开。
- 不要在驱动器带电时插拔电机线，以免电感反冲损坏桥臂。
- 首次上电应给可调电源设置合理的限流值。
- 使用普通接地示波器时，不要把地夹直接接到 OUTA、OUTB 或 OUTC 等开关节点，否则可能通过保护地短路桥臂。
- 测量两个桥臂之间的电压应使用差分探头，或先测 MCU 到 DRV8313 的逻辑 PWM。
- 三路 PWM 正常情况下约为 25 kHz，占空比随电角度变化。
- `nFAULT` 应保持非故障状态；若周期性触发，应检查过流、欠压、过热和输出短路。

## 10. 快速故障判断表

| 现象 | 优先检查 |
|---|---|
| 完全不转，手转只有轻微阻力 | `loopFOC()`、扭矩模式、驱动初始化、EN |
| `target_velocity=0` 能保持，运动时左右摆 | 绕组配对、公共 OUTC、缺相、相位测试 |
| 一个固定电角度保持强，另一个很弱 | 对应 PWM、EN、桥臂、绕组开路或接线 |
| 两个固定角度都保持，但离散步进来回摆 | 公共端接了同一绕组、绕组配对错误 |
| 低速正常，高速失步 | 电压裕量、绕组电感、加速度、负载 |
| 静止正常，直接给高速只抖动 | 未对齐、没有速度斜坡、超过拉入速度 |
| 需要外力推动后才能持续旋转 | 开环未拉入同步、共振、缺相 |
| 电源进入 CC 或 VM 电压下降 | 电压上限过高、绕组电阻过低、输出短路 |

## 11. 本次调试结论

本次问题最终通过以下四项修正解决：

1. 恢复 `motor.loopFOC()`；
2. 使用 `TorqueControlType::voltage`；
3. 按三半桥混合式步进拓扑连接电机，将两个不同绕组的 `A2 + B2` 接在一起连接 OUTC；
4. 初始化驱动器时，将控制公共桥臂的 PA8 严格放在 `BLDCDriver3PWM` 的第三个 PWM 参数，使用 `BLDCDriver3PWM(PB1, PB0, PA8, PB3)`。

其中最容易误判的是第三、四项：静态有保持力并不能证明两相绕组配对正确。对三半桥方案而言，必须先用万用表识别两组独立绕组、建立正确的公共端，再把控制该公共桥臂的引脚放到驱动构造函数第三位。

## 12. AS5147 磁编码器与闭环速度调试

### 12.1 当前硬件和程序配置

当前使用 AS5147 绝对式磁编码器，配置为：

```cpp
MagneticSensorSPI sensor(AS5147_SPI, PC13);

// STM32F103 SPI1
// MOSI=PA7, MISO=PA6, SCK=PA5
SPIClass encoder_spi(PA7, PA6, PA5);
```

PA7、PA6、PA5 是本项目使用的 SPI1 引脚。`encoder_spi` 只是程序对象名称，不代表第二路 SPI。

编码器和电机的初始化顺序必须为：

```text
sensor.init(&encoder_spi)
        ↓
motor.linkSensor(&sensor)
        ↓
driver.init() / motor.linkDriver()
        ↓
motor.init()
        ↓
motor.initFOC()
        ↓
loopFOC() + move()
```

关键代码为：

```cpp
sensor.init(&encoder_spi);
motor.linkSensor(&sensor);

motor.controller = MotionControlType::velocity;
motor.torque_controller = TorqueControlType::voltage;

motor.init();
motor.initFOC();
```

接入编码器并使用 `MotionControlType::velocity` 后，`motor.initFOC()` 不再是可选项。它负责检测编码器方向并确定电角度零点。对齐时电机会转动一小段，因此机械轴必须能够安全活动。

程序已经检查以下返回值：

```cpp
driver.init();
motor.init();
motor.initFOC();
```

如果传感器对齐失败，程序会关闭电机并停止运行，避免在角度未知时继续输出电压。不能只根据串口是否出现 `Motor ready.` 判断初始化是否成功。

### 12.2 SimpleFOC 现成监控功能

程序使用 SimpleFOC 自带的监控变量：

```cpp
motor.useMonitoring(Serial);
motor.monitor_variables =
    _MON_TARGET | _MON_VOLT_Q | _MON_VEL | _MON_ANGLE;
motor.monitor_downsample = 1;
motor.monitor_decimals = 4;
```

主循环中每约 100 ms 调用一次：

```cpp
motor.monitor();
```

串口每行字段顺序为：

```text
目标速度(rad/s)    q轴电压(V)    编码器速度(rad/s)    累计机械角度(rad)
```

例如：

```text
10.0000    2.5293    9.5449    229.8563
```

含义是：

- 目标机械速度为 10 rad/s；
- 当前控制器输出的 q 轴电压为 2.5293 V；
- 编码器估算速度为 9.5449 rad/s；
- 编码器累计机械角度为 229.8563 rad。

`_MON_ANGLE` 输出的是经过传感器方向和零偏处理的累计机械角度，不局限于 `0～2π`。

SimpleFOC 还提供以下直接读取接口：

```cpp
sensor.getMechanicalAngle(); // 单圈机械角度，0～2π
sensor.getAngle();           // 包含完整圈数的累计角度
sensor.getVelocity();        // 编码器估算速度，rad/s
sensor.getFullRotations();   // 完整旋转圈数
```

在正常 FOC 循环中不要额外调用 `sensor.update()`，因为 `motor.loopFOC()` 已经会更新传感器。重复更新会增加 SPI 负担并降低 FOC 循环频率。

### 12.3 速度差分噪声及当前滤波配置

AS5147 是 14 位编码器，单个计数对应的角度约为：

```text
2π / 16384 ≈ 0.000383 rad
```

速度由角度差除以采样时间得到。如果采样时间太短，只有一个计数的变化也会被放大成明显的瞬时速度跳动。

为降低速度量化噪声，当前配置为：

```cpp
sensor.min_elapsed_time = 0.002f; // 速度估算最小间隔 2 ms
motor.LPF_velocity.Tf = 0.02f;    // 速度低通时间常数 20 ms
```

`sensor.min_elapsed_time` 只限制速度差分的更新时间，不会降低 `loopFOC()` 使用机械角度的更新频率。

在目标速度为 5 rad/s 的一组测试中，瞬时值约在 4.3～5.5 rad/s 之间变化，但 13 个样本的平均速度约为：

```text
4.991 rad/s
```

相对目标值的平均误差约为 -0.18%。这说明平均速度控制正确，瞬时波动中包含编码器差分噪声和少量实际速度纹波，不能仅凭单个速度样本判断电机失控。

### 12.4 目标 10 rad/s 时的每圈周期性速度尖峰

在目标速度为 10 rad/s 时观察到明显速度尖峰：

```text
10.0000    1.8611    15.3336    232.3559
10.0000    1.9850    15.9675    238.2245
10.0000    2.2189    14.2754    244.0372
```

这些峰值之间的时间间隔约为 0.58～0.62 秒。目标速度为 10 rad/s 时，一圈的理论时间为：

```text
2π / 10 ≈ 0.628 s
```

因此尖峰近似每机械转一圈重复一次，不像随机 SPI 噪声，更像编码器单圈周期性角度误差。

整段数据的平均速度约为：

```text
(244.0372 - 229.8563) / 1.424 ≈ 9.96 rad/s
```

平均速度仍然接近目标值。与此同时，`voltage_q` 只在约 1.86～2.56 V 之间变化，明显低于 5 V 的 `motor.voltage_limit`，所以该现象不是控制电压饱和，也不应通过继续提高电压上限解决。

当编码器报告速度突然升高时，闭环控制器会降低 `voltage_q`。如果尖峰来自编码器角度非线性，控制器就会追逐错误的速度反馈，进而制造真实的转矩和速度波动。

### 12.5 为什么 50 极对电机对编码器误差特别敏感

FOC 使用下式从机械角度计算电角度：

```text
电角度 = 机械角度 × 极对数 - 电角度零偏
```

本项目使用：

```cpp
HybridStepperMotor motor(50);
```

因此编码器机械角度误差会被放大 50 倍。例如：

```text
机械角度误差 1° → 电角度误差约 50°
```

即使编码器角度曲线只有较小的单圈非线性，也可能在高极对数步进电机上形成明显转矩纹波和速度振荡。`initFOC()` 只能校准方向和一个电角度零点，不能补偿整圈角度非线性。

### 12.6 AS5147 磁铁安装检查

出现每圈重复一次的速度误差时，应优先检查编码器机械和磁路安装，而不是先大幅调整 PID。

检查项目：

1. 磁铁必须为适合该类角度传感器的直径方向充磁磁铁；
2. 磁铁几何中心应尽量对准 AS5147 感应中心；
3. 磁铁表面与芯片表面应保持平行；
4. 轴向气隙应稳定，转动时不能周期性变化；
5. 磁铁、联轴器、转轴和编码器电路板不能松动；
6. 避免铁质螺钉或其他铁磁材料紧邻磁铁和芯片；
7. 检查编码器电源去耦、SPI 接地和连接线；
8. 如果误差每次都出现在相同单圈角度，优先怀疑偏心或磁场畸变。

PC13 在 Blue Pill 上通常还连接板载 LED，并且驱动能力和速度特性不如普通 GPIO。当前 SPI 通信若一直稳定，可以继续使用；若出现随机角度跳变、对齐偶发失败或 SPI 读数异常，应将 CS 改到普通 GPIO 后重新验证。

### 12.7 区分速度估算噪声、PID 振荡和磁编码器误差

| 现象 | 更可能的原因 |
|---|---|
| 瞬时速度跳动，但长时间平均速度准确 | 编码器差分量化噪声 |
| 速度误差以每机械圈一次重复 | 磁铁偏心、倾斜、气隙变化或单圈非线性 |
| 速度升高时 `voltage_q` 降低 | 闭环控制器正在响应速度反馈 |
| `voltage_q` 经常达到 `±voltage_limit` | 电压饱和、负载过大或速度过高 |
| `voltage_q` 未饱和但转矩明显周期性变化 | 编码器角度误差或机械周期负载 |
| 误差与机械角度无关、频率固定 | PID 参数、机械共振或电源问题 |
| 角度出现不连续大跳变 | SPI、CS、供电、接地或编码器硬件故障 |

推荐排查顺序：

1. 先确认磁铁类型、同心度、平行度和气隙；
2. 观察速度尖峰是否总在相同单圈角度重复；
3. 检查 AS5147 的磁场强度和诊断状态；
4. 确认 `voltage_q` 是否达到电压上限；
5. 确认编码器角度可信后，再调整 `PID_velocity.P/I`；
6. 若机械安装无法进一步改善，可考虑建立单圈角度误差查表补偿。
