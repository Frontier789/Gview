#version 430

#define RK_S ?
#define NODES ?
#define RK_C_H ?
#define RK_C_E ?
#define LOCAL_SIZE ?

uniform int scale;

layout(local_size_x = LOCAL_SIZE) in;

layout(std430, binding=4) buffer RKError {
    float rk_error[NODES];
};

void main() {
    const uint lid = gl_LocalInvocationID.x;
    const uint base = lid + gl_WorkGroupID.x * LOCAL_SIZE * scale;
    
    for(int s=1;s<LOCAL_SIZE;s*=2) {
        if (lid % (2*s) == 0 && lid + s < LOCAL_SIZE && base + s*scale < NODES) {
            rk_error[base] = max(rk_error[base], rk_error[base + s*scale]);
        }
        
        barrier();
    }
}
