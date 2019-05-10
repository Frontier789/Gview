#version 430

#define RK_S ?
#define NODES ?
#define RK_C_H ?
#define RK_C_E ?

uniform float h;

layout(local_size_x = 1) in;

layout(std430, binding=0) readonly buffer Positions {
    vec2 positions[NODES];
};

layout(std430, binding=2) readonly buffer K {
    vec2 k[NODES][RK_S];
};

layout(std430, binding=3) writeonly buffer PosOut {
    vec2 posE[NODES];
};

layout(std430, binding=4) writeonly buffer RKError {
    float rk_error[NODES];
};

void main() {
    const uint wid = gl_WorkGroupID.x;
    
    const float[RK_S] rk_C_H = float[RK_S](RK_C_H);
    const float[RK_S] rk_C_E = float[RK_S](RK_C_E);
    
    // mix k-s to get new positions
    vec2 pO = positions[wid];
    vec2 pH = pO;
    vec2 pE = pO;
    for (int s=0;s<RK_S;s++) {
        pH += h*rk_C_H[s]*k[wid][s];
        pE += h*rk_C_E[s]*k[wid][s];
    }
    
    posE[wid] = pE;
    rk_error[wid] = length(pH - pE);
}
