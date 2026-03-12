Class StateMachine
{
    // 状态列表
    States: List<State>
    // 当前激活状态
    CurrentState: State  
    // 状态机入口状态
    EntryState: State 

    Function Initialize()
    {
        CurrentState = EntryState
    }

    Function Update(inputVariables)
    {
        // 先检查是否有满足条件的转换发生
        Foreach transition in CurrentState.OutgoingTransitions
        {
            If transition.CanTransition(inputVariables) == true
            {
                CurrentState.OnExit()
                CurrentState = transition.TargetState
                CurrentState.OnEnter()
                Break
            }
        }
        CurrentState.UpdateState(inputVariables)
    }

    Function GetOutputPose()
    {
        Return CurrentState.GetPose()
    }
}


Class State
{
    Name: string
    // 播放动画相关逻辑或节点图
    AnimationLogic: AnimGraphLogic  
    // 某个状态（State）向外连接的所有转换（Transition）集合。
    // 每个Transition定义了从当前状态到另一个目标状态的转换条件和逻辑。状态机每帧都会遍历这些
    OutgoingTransitions: List<Transition>

    Function OnEnter()
    {
        // 进入状态时初始化，比如重置动画时间
        AnimationLogic.Reset()
    }

    Function OnExit()
    {
        // 离开状态时清理或保存状态
    }

    Function UpdateState(inputVariables)
    {
        AnimationLogic.Update(inputVariables)
    }

    Function GetPose()
    {
        Return AnimationLogic.EvaluatePose()
    }
}


Class Transition
{
    FromState: State
    TargetState: State
    Condition: Function(inputVariables) -> bool

    Function CanTransition(inputVariables)
    {
        Return Condition(inputVariables)
    }
}

Class AnimGraphLogic
{
    // 包含的动画节点，例如SequencePlayer、BlendSpace等
    AnimationNodes: List<AnimNode>
    // 当前动画时间，控制播放进度
    CurrentTime: float                
    Function Reset()
    {
        CurrentTime = 0
        Foreach node in AnimationNodes
        {
            node.Reset()
        }
    }

    Function Update(inputVariables)
    {
        Foreach node in AnimationNodes
        {
            node.Update(inputVariables, CurrentTime)
        }
        CurrentTime += DeltaTime
    }

    Function EvaluatePose()
    {
        pose = AnimPose()
        Foreach node in AnimationNodes
        {
            pose = BlendPoses(pose, node.GetPose())
        }
        Return pose
    }
}

main() 
{
    // 使用示例
    stateMachine = new StateMachine()
    stateMachine.EntryState = idleState
    stateMachine.Initialize()
	Loop EveryFrame:
    {
        // e.g. speed, isJumping, isFalling...
        inputVars = GetAnimationInputVariables()
        // 处理状态转换和动画更新
        stateMachine.Update(inputVars)
        // 获取当前骨骼姿势输出
        pose = stateMachine.GetOutputPose()
        ApplyPoseToCharacter(pose)
    }
}
















