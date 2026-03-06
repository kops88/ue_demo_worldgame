# ALSBaseCharacter 数据结构

## 枚举类型 (EALS*)

### EALSGait - 步态状态  //最常用的形态 - 行走
| 值 | 说明 |
|---|---|
| Walking | 行走 |
| Running | 跑步 |
| Sprinting | 冲刺 |

### EALSMovementAction - 移动动作 // 独立的小动作
| 值 | 说明 |
|---|---|
| None | 无 |
| LowMantle | 低处攀爬 |
| HighMantle | 高处攀爬 |
| Rolling | 翻滚 |
| GettingUp | 起身 |

### EALSMovementState - 移动状态  //长期使用的状态
| 值 | 说明 |
|---|---|
| None | 无 |
| Grounded | 地面 |
| InAir | 空中 |
| Mantling | 攀爬中 |
| Ragdoll | 布娃娃 |

### EALSOverlayState - 覆盖状态  //角色动作
| 值 | 说明 |
|---|---|
| Default | 默认 |
| Masculine | 男性化 |
| Feminine | 女性化 |
| Injured | 受伤 |
| HandsTied | 双手被绑 |
| Rifle | 步枪 |
| PistolOneHanded | 单手手枪 |
| PistolTwoHanded | 双手手枪 |
| Bow | 弓箭 |
| Torch | 火把 |
| Binoculars | 望远镜 |
| Box | 箱子 |
| Barrel | 桶 |

### EALSRotationMode - 旋转模式  //切换角色朝向模式
| 值 | 说明 |
|---|---|
| VelocityDirection | 速度方向 |
| LookingDirection | 视角方向 |
| Aiming | 瞄准 |

### EALSStance - 姿态 //人物形体状态
| 值 | 说明 |
|---|---|
| Standing | 站立 |
| Crouching | 蹲伏 |

### EALSViewMode - 视角模式 //模式切换
| 值 | 说明 |
|---|---|
| ThirdPerson | 第三人称 |
| FirstPerson | 第一人称 |

### EALSGroundedEntryState - 地面进入状态
| 值 | 说明 |
|---|---|
| None | 无 |
| Roll | 翻滚 |

---

## 结构体 (FALS*)

### FALSMovementSettings - 移动设置
| 成员 | 类型 | 说明 |
|------|------|------|
| WalkSpeed | float | 行走速度 |
| RunSpeed | float | 跑步速度 |
| SprintSpeed | float | 冲刺速度 |
| ***MovementCurve*** | UCurveVector* | 移动曲线 |
| ***RotationRateCurve*** | UCurveFloat* | 旋转速率曲线 |

### FALSMovementStateSettings - 移动状态设置 (继承FTableRowBase)
| 成员 | 类型 | 说明 |
|------|------|------|
| VelocityDirection | FALSMovementStanceSettings | 速度方向设置 |
| LookingDirection | FALSMovementStanceSettings | 视角方向设置 |
| Aiming | FALSMovementStanceSettings | 瞄准设置 |

### FALSMantleAsset - 攀爬资源
| 成员 | 类型 | 说明 |
|------|------|------|
| AnimMontage | UAnimMontage* | 攀爬动画 |
| PositionCorrectionCurve | UCurveVector* | 位置修正曲线 |
| StartingOffset | FVector | 起始偏移 |
| LowHeight | float | 低处高度 |
| LowPlayRate | float | 低处播放速率 |
| LowStartPosition | float | 低处起始位置 |
| HighHeight | float | 高处高度 |
| HighPlayRate | float | 高处播放速率 |
| HighStartPosition | float | 高处起始位置 |

### FALSCameraSettings - 相机设置
| 成员 | 类型 | 说明 |
|------|------|------|
| TargetArmLength | float | 相机臂长度 |
| SocketOffset | FVector | 插槽偏移 |
| LagSpeed | float | 滞后速度 |
| RotationLagSpeed | float | 旋转滞后速度 |
| bDoCollisionTest | bool | 是否进行碰撞测试 |

---

## 成员变量

### Input 输入
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| DesiredRotationMode | EALSRotationMode | LookingDirection | 期望旋转模式 |
| DesiredGait | EALSGait | Running | 期望步态 |
| DesiredStance | EALSStance | Standing | 期望姿态 |
| LookUpDownRate | float | 1.25f | 上下视角速率 |
| LookLeftRightRate | float | 1.25f | 左右视角速率 |
| RollDoubleTapTimeout | float | 0.3f | 翻滚双击超时 |
| bBreakFall | bool | false | 是否正在破摔 |
| bSprintHeld | bool | false | 冲刺键是否按住 |

### Camera System 相机系统
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| ThirdPersonFOV | float | 90.0f | 第三人称FOV |
| FirstPersonFOV | float | 90.0f | 第一人称FOV |
| bRightShoulder | bool | false | 右肩视角 |
| CameraBehavior | UALSPlayerCameraBehavior* | nullptr | 相机行为实例 |

### Movement System 移动系统
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| MovementModel | FDataTableRowHandle | - | 移动模型数据表 |
| MovementData | FALSMovementStateSettings | - | 移动数据 |

### Essential Information 基础信息
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| Acceleration | FVector | ZeroVector | 加速度 |
| bIsMoving | bool | false | 是否移动中 |
| bHasMovementInput | bool | false | 是否有移动输入 |
| LastVelocityRotation | FRotator | - | 上次速度朝向 |
| LastMovementInputRotation | FRotator | - | 上次移动输入朝向 |
| Speed | float | 0.0f | 当前速度 |
| MovementInputAmount | float | 0.0f | 移动输入量 |
| AimYawRate | float | 0.0f | 瞄准偏航速率 |
| EasedMaxAcceleration | float | 0.0f | 缓动最大加速度 |
| ReplicatedCurrentAcceleration | FVector | ZeroVector | 复制的当前加速度 |
| ReplicatedControlRotation | FRotator | ZeroRotator | 复制的控制器旋转 |

### State Values 状态值
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| OverlayState | EALSOverlayState | Default | 覆盖状态 |
| GroundedEntryState | EALSGroundedEntryState | - | 地面进入状态 |
| MovementState | EALSMovementState | None | 移动状态 |
| PrevMovementState | EALSMovementState | None | 上一个移动状态 |
| MovementAction | EALSMovementAction | None | 移动动作 |
| RotationMode | EALSRotationMode | LookingDirection | 旋转模式 |
| Gait | EALSGait | Walking | 当前步态 |
| Stance | EALSStance | Standing | 当前姿态 |
| ViewMode | EALSViewMode | ThirdPerson | 视角模式 |
| OverlayOverrideState | int32 | 0 | 覆盖覆盖状态 |

### Rotation System 旋转系统
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| TargetRotation | FRotator | ZeroRotator | 目标旋转 |
| InAirRotation | FRotator | ZeroRotator | 空中旋转 |
| YawOffset | float | 0.0f | 偏航偏移 |
| AimingRotation | FRotator | ZeroRotator | 瞄准旋转 |

### Breakfall System 破摔系统
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| bBreakfallOnLand | bool | true | 落地时破摔 |
| BreakfallOnLandVelocity | float | 700.0f | 破摔落地速度阈值 |

### Ragdoll System 布娃娃系统
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| bReversedPelvis | bool | false | 是否反转骨盆 |
| bRagdollOnLand | bool | false | 落地时布娃娃 |
| RagdollOnLandVelocity | float | 1000.0f | 布娃娃落地速度阈值 |
| bRagdollOnGround | bool | false | 是否在地面上布娃娃 |
| bRagdollFaceUp | bool | false | 布娃娃是否面朝上 |
| LastRagdollVelocity | FVector | ZeroVector | 上次布娃娃速度 |
| TargetRagdollLocation | FVector | ZeroVector | 目标布娃娃位置 |
| ServerRagdollPull | float | 0.0f | 服务器布娃娃拉力 |

### Other 其他
| 变量 | 类型 | 默认值 | 说明 |
|------|------|--------|------|
| MyCharacterMovementComponent | UALSCharacterMovementComponent* | - | 自定义移动组件 |
| VisibleMesh | USkeletalMesh* | nullptr | 可见骨架网格体 |
| PreviousVelocity | FVector | ZeroVector | 上次速度 |
| PreviousAimYaw | float | 0.0f | 上次瞄准偏航 |
| LastStanceInputTime | float | 0.0f | 上次姿态输入时间 |
| OnLandedFrictionResetTimer | FTimerHandle | - | 落地摩擦重置计时器 |
| bEnableNetworkOptimizations | bool | false | 启用网络优化 |
| ALSDebugComponent | UALSDebugComponent* | nullptr | 调试组件 |

---

## 主要函数

### Movement State 移动状态
- `SetMovementState(NewState, bForce)` - 设置移动状态
- `GetMovementState()` - 获取移动状态
- `GetPrevMovementState()` - 获取上一个移动状态
- `SetMovementAction(NewAction, bForce)` - 设置移动动作
- `GetMovementAction()` - 获取移动动作

### Stance 姿态
- `SetStance(NewStance, bForce)` - 设置姿态
- `GetStance()` - 获取姿态
- `SetDesiredStance(NewStance)` - 设置期望姿态
- `GetDesiredStance()` - 获取期望姿态

### Gait 步态
- `SetGait(NewGait, bForce)` - 设置步态
- `GetGait()` - 获取步态
- `GetDesiredGait()` - 获取期望步态
- `SetDesiredGait(NewGait)` - 设置期望步态
- `GetAllowedGait()` - 获取允许的步态
- `GetActualGait(AllowedGait)` - 获取实际步态
- `CanSprint()` - 是否可以冲刺

### Rotation Mode 旋转模式
- `SetRotationMode(NewRotationMode, bForce)` - 设置旋转模式
- `GetRotationMode()` - 获取旋转模式
- `SetDesiredRotationMode(NewRotMode)` - 设置期望旋转模式
- `GetDesiredRotationMode()` - 获取期望旋转模式

### View Mode 视角模式
- `SetViewMode(NewViewMode, bForce)` - 设置视角模式
- `GetViewMode()` - 获取视角模式

### Overlay State 覆盖状态
- `SetOverlayState(NewState, bForce)` - 设置覆盖状态
- `GetOverlayState()` - 获取覆盖状态
- `SetOverlayOverrideState(NewState)` - 设置覆盖覆盖状态
- `GetOverlayOverrideState()` - 获取覆盖覆盖状态

### Ragdoll System 布娃娃系统
- `RagdollStart()` - 开始布娃娃
- `RagdollEnd()` - 结束布娃娃
- `GetGetUpAnimation(bRagdollFaceUpState)` - 获取起身动画(BP实现)

### Camera 相机
- `IsRightShoulder()` - 是否右肩视角
- `SetRightShoulder(bNewRightShoulder)` - 设置右肩视角
- `GetThirdPersonTraceParams(TraceOrigin, TraceRadius)` - 获取第三人称追踪参数
- `GetThirdPersonPivotTarget()` - 获取第三人称枢轴目标
- `GetFirstPersonCameraTarget()` - 获取第一人称相机目标
- `GetCameraParameters(TPFOVOut, FPFOVOut, bRightShoulderOut)` - 获取相机参数
- `SetCameraBehavior(CamBeh)` - 设置相机行为

### Input 输入
- `ForwardMovementAction(Value)` - 前进移动
- `RightMovementAction(Value)` - 右移
- `CameraUpAction(Value)` - 相机上移
- `CameraRightAction(Value)` - 相机右移
- `JumpAction(bValue)` - 跳跃
- `SprintAction(bValue)` - 冲刺
- `AimAction(bValue)` - 瞄准
- `CameraTapAction()` - 相机点击
- `CameraHeldAction()` - 相机按住
- `StanceAction()` - 姿态切换
- `WalkAction()` - 行走切换
- `RagdollAction()` - 布娃娃
- `VelocityDirectionAction()` - 速度方向模式
- `LookingDirectionAction()` - 视角方向模式

### Essential Information 基础信息
- `GetAcceleration()` - 获取加速度
- `IsMoving()` - 是否移动
- `GetMovementInput()` - 获取移动输入
- `GetMovementInputAmount()` - 获取移动输入量
- `GetSpeed()` - 获取速度
- `GetAimingRotation()` - 获取瞄准旋转
- `GetAimYawRate()` - 获取瞄准偏航速率

### Utility 工具
- `GetAnimCurveValue(CurveName)` - 获取动画曲线值
- `SetVisibleMesh(NewSkeletalMesh)` - 设置可见网格体
- `GetMyMovementComponent()` - 获取移动组件
- `GetTargetMovementSettings()` - 获取目标移动设置
- `GetRollAnimation()` - 获取翻滚动画(BP实现)

### Movement 移动
- `OnBreakfall()` - 破摔开始(BP可实现)
- `Replicated_PlayMontage(Montage, PlayRate)` - 复制播放蒙太奇

### Delegates 委托
- `JumpPressedDelegate` - 跳跃按下委托
- `OnJumpedDelegate` - 已跳跃委托
- `RagdollStateChangedDelegate` - 布娃娃状态变化委托
