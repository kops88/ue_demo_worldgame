





请给我列举出ue5中关于actor的基础功能和核心功能, 希望包括常用的, 和虽然不常用但是对理解ue5运作非常重要的

根据实际支持这些功能的实际代码, 简单写出支持这些功能的伪代码



### 4. Actor的创建（Spawn）与销毁（Elimination）

+ Actor通过 `SpawnActor()` 在运行时动态生成。
+ 使用 `Destroy()` 函数或标记为 PendingKill 进入垃圾回收，被系统安全消除。
+ 运行中创建和销毁支持游戏玩法动态性。

### 5. Actor生命周期

+ **加载（Load From Disk）** ：从关卡数据加载Actor，执行 `PostLoad()`, `PreInitializeComponents()`, `InitializeComponent()`，`PostInitializeComponents()`，`BeginPlay()`。
+ **编辑中播放（Play In Editor）** ：Actor在编辑器复制，运行时初始化类似加载流程。
+ **运行阶段** ：响应游戏逻辑、碰撞、事件，支持Tick，执行行为。
+ **销毁** ：标记为PendingKill，执行GC清理。

### 6. Actor层级与组件附件

+ Actor的组件可组成层级结构（父子附加）。
+ RootComponent为根节点，影响整个Actor变换。
+ 多组件协同实现复合功能，如碰撞触发器、视觉特效和音频叠加。

### 7. 重要但较少被注意的功能

+ **网络复制（Replication）**
  + Actor支持属性与函数跨网络同步。
  + 通过标记 `Replicated` 属性和RPC函数保障网络一致性。

+ **类型转换（Casting）**
  + 可将Actor对象转换为具体子类以调用特有功能。
  + 用于多态行为处理，如判断重叠对象具体类型。

+ **垃圾回收与内存管理**
  + Actor及其组件通过UE5的GC系统管理。
  + 需用UPROPERTY保护UObject指针，避免提前清理。

+ **生命周期回调函数**
  + 如 `BeginPlay()`, `Tick()`, `EndPlay()`等，控制时间相关行为。

+ **事件系统与接口支持**
  + Actor可通过事件和接口与其他系统通信。

### 8. 案例说明

假设创建一个带有碰撞盒和视觉网格的拾取物体Actor：

+ RootComponent为USceneComponent定位物体。
+ StaticMeshComponent 作为视觉展示。
+ BoxComponent 作为触发器检测玩家重叠事件。
+ 通过覆盖 `NotifyActorBeginOverlap` 实现拾取逻辑。
