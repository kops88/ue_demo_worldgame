

验证项目目前规模小（4 个 Slot、同步操作、单角色）。以下是针对每个技术点"人为制造需求"的方案，让你有合理的理由去实现这些技术。

---

### 方案 1："异步流程管理"

**核心思路：让资源不再提前设好，而是运行时从磁盘异步加载。**

| 改动 | 具体做法 |
|------|---------|
| 准备多套 Mesh 资源 | 用 UE5 的 MetaHuman 或者从商城下载 2-3 套不同的角色 Mesh（不同体型/外形），放在不同目录下 |
| 去掉蓝图预设 Mesh | Slave 组件在蓝图中**不设置** SkeletalMesh，只保留空壳组件 |
| 换装时异步加载 | 把 `SetSkeletalMesh(NewMesh)` 改为 `StreamableManager.RequestAsyncLoad(MeshPath, Callback)`，在回调中才 `SetSkeletalMesh` |
| 制造竞态 | 在加载尚未完成时，再次触发换装（快速连按换装键），你就会遇到"上一次加载还没完，新请求来了"的问题 |

**这时你就需要**：

- 一个状态变量记录"当前在加载中还是已完成"
- 在新请求到来时取消上一次加载
- 加载完成的回调里要检查"这次加载的结果还有效吗"

这就是状态机的雏形。

**具体步骤**：

```cpp
// 1. 在组件中添加 StreamableManager
FStreamableManager StreamableManager;

// 2. SwapSlotMesh 改为异步版本
void SwapSlotMeshAsync(FName SlotName, FSoftObjectPath MeshPath)
{
    // 发起异步加载
    auto Handle = StreamableManager.RequestAsyncLoad(MeshPath, 
        FStreamableDelegate::CreateUObject(this, &UAvatarTestComponent::OnMeshLoaded, SlotName));
    // 问题来了：如果在加载过程中又调了一次 SwapSlotMeshAsync 怎么办？
}
```

---

### 方案 2："增量刷新（脏标记）"

**核心思路：增加 Slot 数量 + 增加一个"数据层"，让换装从"直接调 API"变成"修改数据 → 刷新"。**

| 改动 | 具体做法 |
|------|---------|
| 扩充到 8+ 个 Slot | 除了 UpperBody/LowerBody/Head/Hands，再加 Hair、Helm、Chest、Leg（即使都用 Manny + 不同颜色） |
| 创建一个数据结构 | 定义 `FAvatarData`：包含每个 Slot 当前用的 Mesh 路径和材质路径 |
| 换装改为"修改数据 → 调 RefreshAvatar()" | 不再直接 `SwapSlotMesh`，而是修改 `AvatarData.HelmMesh = 新路径`，然后调 `RefreshAvatar()` |
| 全量 vs 增量 | 第一版 `RefreshAvatar()` 全量刷新所有 8 个 Slot → 加一个计时器打印耗时 → 发现"只换了头盔但刷了 8 个部位很浪费" |

**这时你就需要**：
- 对 `FAvatarData` 的每个属性记录"上次刷新时的值"
- `RefreshAvatar()` 时对比新旧值，只刷新变化的 Slot

```cpp
// 数据结构
struct FAvatarData {
    FSoftObjectPath BodyMesh;
    FSoftObjectPath HelmMesh;
    FSoftObjectPath ChestMesh;
    // ... 8 个部位
};

// 脏标记
struct FDirtyFlags {
    bool bBodyDirty = false;
    bool bHelmDirty = false;
    bool bChestDirty = false;
    // ...
};

// 修改数据时标记脏
void SetHelmMesh(FSoftObjectPath NewPath) {
    if (AvatarData.HelmMesh != NewPath) {
        AvatarData.HelmMesh = NewPath;
        DirtyFlags.bHelmDirty = true;
    }
}

// RefreshAvatar 只刷新脏了的
void RefreshAvatar() {
    if (DirtyFlags.bHelmDirty) {
        LoadAndApply("Helm", AvatarData.HelmMesh);
        DirtyFlags.bHelmDirty = false;
    }
    // ... 其他部位
}
```

---

### 方案 3："MeshMerge 合批"

**核心思路：增加角色数量，用 `stat scenerendering` 观察 DrawCall。**

| 改动 | 具体做法 |
|------|---------|
| 场景中放 20-50 个角色 | 在关卡中复制 20-50 个 `BP_AvatarTestCharacter`，每个都有 4 个 Slave |
| 打开 DrawCall 统计 | 控制台输入 `stat scenerendering`，观察 **Mesh Draw Calls** |
| 对比数据 | 50 角色 × 4 Slave = 200 DrawCall（仅角色部分） |
| 实现合批 | 用 UE5 的 `USkeletalMeshComponent::MergeMeshes()` 或自己实现简化版：把同一角色的 4 个 Slave Mesh 合成 1 个 |

**简化版合批实现思路**：

```cpp
// UE5 提供的 API（简化版）
// FSkeletalMeshMerge 可以将多个 SkeletalMesh 合并为一个

void MergeSlaves() {
    TArray<USkeletalMeshComponent*> ComponentsToMerge;
    for (auto& [Name, Comp] : SlaveMap)
        ComponentsToMerge.Add(Comp);
    
    // 合并后得到一个新的 SkeletalMesh
    USkeletalMesh* MergedMesh = MergeMeshUtility::MergeMeshes(ComponentsToMerge);
    
    // 隐藏原始 Slave，显示合并后的 Mesh
    for (auto& [Name, Comp] : SlaveMap)
        Comp->SetVisibility(false);
    MergedMeshComponent->SetSkeletalMesh(MergedMesh);
    MergedMeshComponent->SetVisibility(true);
}
```

**观察效果**：DrawCall 从 200 降到 50（每角色只有 1 个合批 Mesh）。

---

### 方案 4："材质参数批次叠加"

**核心思路：给角色加多个"材质修改来源"，让它们需要同时存在。**

| 改动 | 具体做法 |
|------|---------|
| 制作一个支持参数的材质 | 把 `M_Solid_Red` 改为参数化材质：用 `MaterialParameterCollection` 或 `ScalarParameter` 控制颜色 |
| 添加多个修改来源 | **来源 A**：玩家选择的基础颜色（红/蓝/绿）；**来源 B**：中毒 Buff（叠加绿色）；**来源 C**：被击中闪白（临时全白） |
| 多来源叠加场景 | 角色基础色是红色 → 中毒了叠加绿色 → 被打了闪白 → 闪白结束恢复绿色 → 中毒结束恢复红色 |

**如果不做分层管理**，你会发现：
- 中毒结束时，怎么知道要恢复到什么颜色？（是红色还是闪白中的白色？）
- 闪白和中毒同时存在时，谁的优先级高？
- 如何平滑过渡（渐变回原色，而不是突然切换）？

```cpp
// 材质参数修改批次
struct FMaterialParamBatch {
    FName Tag;           // 来源标识："BaseColor" / "Poison" / "HitFlash"
    int32 Priority;      // 优先级：HitFlash(100) > Poison(50) > BaseColor(0)
    float BlendAlpha;    // 混合比例 0~1（用于渐变）
    FLinearColor Color;  // 目标颜色
};

// 添加修改
void AddMaterialBatch(FMaterialParamBatch Batch);
// 移除修改（按 Tag）
void RemoveMaterialBatch(FName Tag);
// 重新计算最终颜色（按优先级排序，逐层混合）
FLinearColor CalculateFinalColor();
```

**具体场景**：
1. 按键 1：设置基础色为红色（Priority=0）
2. 按键 2：添加"中毒"效果，BlendAlpha 从 0 渐变到 0.5（Priority=50）→ 角色变成红绿混合
3. 按键 3：添加"被击中"闪白，BlendAlpha=1.0（Priority=100）→ 角色全白
4. 0.2 秒后闪白自动移除 → 恢复红绿混合
5. 5 秒后中毒解除 → 恢复纯红色

---

### 方案 5："动画替换优先级"

**核心思路：准备同一动画的多个版本，让系统根据角色属性选择最匹配的。**

| 改动 | 具体做法 |
|------|---------|
| 准备多版本动画 | 用 UE5 的 Mannequin 动画复制出 3 份 Idle 动画，放在不同路径下 |
| 设定路径规则 | `/Common/Animation/Idle` → 默认版；`/StrongMale/Animation/Idle` → 壮男版；`/Adventurer_01/Animation/Idle` → 冒险家 1 专属版 |
| 给角色加属性 | 在组件中加 `EBodyType BodyType` 和 `int32 AdventurerID` |
| 实现替换逻辑 | 播放动画前，根据角色属性查找"有没有更匹配的版本" |

```cpp
// 动画替换决策
UAnimSequence* GetBestAnimation(FSoftObjectPath BaseAnimPath) {
    // 优先级 1：冒险家专属
    if (AdventurerID > 0) {
        FString CustomPath = BaseAnimPath.Replace("/Common/", 
            FString::Printf("/Adventurer_%d/", AdventurerID));
        if (DoesAssetExist(CustomPath))
            return LoadAsync(CustomPath);  // 异步加载
    }
    
    // 优先级 2：体型替换
    FString BodyTypePath = BaseAnimPath.Replace("/Common/", 
        GetBodyTypeFolder(BodyType));
    if (DoesAssetExist(BodyTypePath))
        return LoadAsync(BodyTypePath);
    
    // 优先级 3：默认
    return LoadAsync(BaseAnimPath);
}
```

**验证场景**：
- 按键切换角色的 BodyType（Standard → Strong → Girl）
- 观察 Idle 动画是否自动切换到对应版本
- 故意删掉某个版本的动画文件 → 系统是否正确降级到默认版本

**制作多版本动画的简单方法**：
- 不需要真的做不同的动画
- 可以把同一个 Idle 动画复制到不同目录，修改播放速率（壮男版慢 20%，少女版快 10%）
- 这样视觉上就能看出"当前用的是哪个版本"

---

### 方案 6："性能极致优化"

**核心思路：暴力增加角色数量，让性能崩掉，然后逐项优化。**

| 改动 | 具体做法 |
|------|---------|
| 场景放 100 个角色 | 用蓝图 SpawnActor 循环生成 100 个角色 |
| 打开性能统计 | `stat unit`（查看 Game/Draw/GPU ms）、`stat anim`（查看动画耗时） |
| 观察帧率崩塌 | 100 个角色 × 4 Slave × 每帧全量 Tick + 动画求值 → 帧率很可能降到 20fps 以下 |
| 逐项实现优化 | 每实现一个优化，对比前后帧率数据 |

#### 优化实现清单（按投入产出比排序）

##### 6A. 不可见不 Tick（最简单）

```cpp
// 在 Slave 的构造函数中
VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
```

**效果**：背对摄像机的角色不算动画 → 帧率立涨

##### 6B. URO（远处降频）

```cpp
// 在 Master 或 Slave 上启用 URO
bEnableUpdateRateOptimizations = true;
AnimUpdateRateParams->BaseVisibleDistanceFactorThesholds = { 0.4f, 0.2f, 0.1f };
// 距离越远，动画更新频率越低（每 2 帧、每 4 帧更新一次）
```

**效果**：远处角色动画降频 → 大量节省 CPU

##### 6C. 并行动画求值

```cpp
// UE5 默认就支持，但可以显式控制
bAllowMultiThreadedAnimationUpdate = true;
```

**效果**：动画求值分散到多个 CPU 核心

##### 6D. Master 统一调度 Slave

```cpp
// 在 Master 的 TickComponent 中
for (auto* Slave : SlavesToTick) {
    Slave->SetComponentTickEnabled(false);  // 禁止 Slave 自己 Tick
    Slave->TickComponent(DeltaTime);         // Master 代为 Tick
}
```

**效果**：减少引擎 Tick 调度开销（TaskGraph 任务数减少）

##### 6E. 移动降频插值（高级）

```cpp
// 让移动组件每 3 帧 Tick 一次，但 Mesh 每帧平滑插值
// 这个需要较多代码，建议最后实现
```

#### 建议的验证流程

```
                100 个角色 + 4 Slave = 400 组件
                        帧率: 15fps ← 起点
                            │
         ┌──── 6A: 不可见不Tick ────┐
         │       帧率: 25fps        │
         │          │               │
         │    6B: URO 远处降频      │
         │       帧率: 35fps        │
         │          │               │
         │    6C: 并行动画求值      │
         │       帧率: 40fps        │
         │          │               │
         │    6D: Master统一调度    │
         │       帧率: 45fps        │
         │          │               │
         │    6E: 移动降频插值      │
         │       帧率: 50fps        │
         └──────────────────────────┘

每一步都记录帧率数据，形成优化报告
```

---

### 总结：实施优先级建议

| 优先级 | 技术点 | 原因 |
|--------|--------|------|
| ⭐⭐⭐ 第一 | **方案 1：异步加载 + 状态机** | 这是所有后续方案的基础，换装从同步变异步后，自然需要状态管理 |
| ⭐⭐⭐ 第二 | **方案 2：脏标记增量刷新** | 和方案 1 紧密配合，8 个 Slot + 异步加载 → 自然需要知道"刷哪些" |
| ⭐⭐ 第三 | **方案 4：材质参数叠加** | 独立于换装流程，可以随时加入，逻辑自成体系 |
| ⭐⭐ 第四 | **方案 6：性能优化** | 需要先搞定场景生成 100 个角色，然后逐项加优化看效果 |
| ⭐ 第五 | **方案 5：动画替换** | 需要准备多版本动画资源，工作量稍大 |
| ⭐ 第六 | **方案 3：MeshMerge 合批** | 技术复杂度最高，建议最后尝试 |

升级路线：从最简单的异步化开始，逐步增加复杂度，每一步都有明确驱动和可度量的效果。
