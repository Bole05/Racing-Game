#pragma once
namespace CarStats {
    // 动力属性
    const float MAX_SPEED = 10.0f;       // 最高速度
    const float ACCELERATION = 7.0f;    // 加速力 (Torque/Force)
    const float BRAKING_FORCE = 30.0f;   // 刹车力
    const float STEERING_SPEED = 8.3f;  // 转向速度 (弧度)

    // 物理属性 (Box2D)
    const float DENSITY = 1.0f;          // 密度 (影响重量)
    const float FRICTION = 0.3f;         // 摩擦力
    const float RESTITUTION = 0.1f;      // 弹性
    const float LINEAR_DAMPING = 0.2f;   // 线性阻尼 (空气阻力)
    const float ANGULAR_DAMPING = 2.5f;  // 旋转阻尼 (防止乱转)
}