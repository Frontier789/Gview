#version 430

#define RK_S ?
#define NODES ?
#define RK_A ?
#define LOCAL_SIZE ?

uniform float h;
uniform float squeeze;
uniform int k_i;
uniform int acc_start;

layout(local_size_x = LOCAL_SIZE) in;

layout(std430, binding=0) readonly buffer Positions {
    vec2 positions[NODES];
};

layout(std430, binding=1) readonly buffer Edges {
    float edges[NODES][NODES];
};

layout(std430, binding=2) buffer K {
    vec2 k[NODES][RK_S];
};

layout(std430, binding=5) readonly buffer Weights {
    float nodeWts[NODES];
};

vec2 speed_i(in vec2 A,in vec2 B,in uint a,in uint b);
vec2 speed_sq(vec2 p,float strength);
void reduce_kacc(in uint id);

shared vec2 kacc[LOCAL_SIZE];

void main() {
    const uint lid = gl_LocalInvocationID.x;
    const uint nid = gl_LocalInvocationID.x + acc_start;
    const uint wid = gl_WorkGroupID.x;
    
    vec2 force = k[wid][k_i];
    
    if (acc_start == 0) {
        force = speed_sq(positions[wid],squeeze);
    }
    
    vec2 kacc_val = vec2(0,0);

    if (nid < NODES) {
        vec2 posw = positions[wid];
        vec2 posl = positions[nid];
        
        if (k_i > 0) {
            const float[RK_S] rk_A = float[RK_S](RK_A);
            
            posw += rk_A[k_i]*h*k[wid][k_i-1];
            posl += rk_A[k_i]*h*k[nid][k_i-1];
        }
        
        kacc_val = speed_i(posw,posl,wid,nid);
    }
    
    kacc[lid] = kacc_val;
    barrier();
    reduce_kacc(lid);
    
    if (lid == 0) {
        k[wid][k_i] = force + kacc[0];
    }
    
    // const uint lid = gl_LocalInvocationID.x;
    // const uint wid = gl_WorkGroupID.x;

    // if (k_i == 0) {
    //     kacc[lid] = speed_i(positions[wid],positions[lid],wid,lid);
    //     barrier();

    //     reduce_kacc(lid);
    //     k[wid][0] = kacc[0] + speed_sq(positions[wid],squeeze);
    // } else {
    //     const float[RK_S] rk_A = float[RK_S](RK_A);
        
    //     kacc[lid] = speed_i(positions[wid] + rk_A[k_i]*h*k[wid][k_i-1],
    //                         positions[lid] + rk_A[k_i]*h*k[lid][k_i-1],
    //                         wid,lid);
    //     barrier();
        
    //     reduce_kacc(lid);
    //     k[wid][k_i] = kacc[0] + speed_sq(positions[wid],squeeze);
    // }
}

// const uint lid = gl_LocalInvocationID.x;
// const uint wid = gl_WorkGroupID.x;

// if (k_i == 0) {
//     kacc[lid] = speed_i(positions[wid],positions[lid],wid,lid);
//     barrier();

//     reduce_kacc(lid);
//     k[wid][0] = kacc[0] + speed_sq(positions[wid],squeeze);
// } else {
//     const float[RK_S] rk_A = float[RK_S](RK_A);
    
//     kacc[lid] = speed_i(positions[wid] + rk_A[k_i]*h*k[wid][k_i-1],
//                         positions[lid] + rk_A[k_i]*h*k[lid][k_i-1],
//                         wid,lid);
//     barrier();
    
//     reduce_kacc(lid);
//     k[wid][k_i] = kacc[0] + speed_sq(positions[wid],squeeze);
// }

/// Utility

// G = 30*30*ln(30)*2
#define G 6122.16
#define H 1

void reduce_kacc(in uint id)
{
    uint frame = LOCAL_SIZE;
    for (uint s = (LOCAL_SIZE+1)/2; s>0; s=(s+1)/2) {
        if (id < s && s + id < frame)
            kacc[id] += kacc[s + id];
            
        barrier();
        
        frame = s;
        if (s == 1) s = 0;
    }
}

vec2 speed(vec2 A,vec2 B,float m,float strength)
{
	vec2 d = B - A;
    float L = d.x*d.x + d.y*d.y;
    d = normalize(d);
    
	vec2 culomb = -d * G * m / L;
    vec2 hooke  =  d * H * log(L) * strength;
    
    return culomb + hooke;
}

vec2 speed_i(in vec2 A,in vec2 B,in uint a,in uint b)
{
    if (a == b) {
        return vec2(0,0);
    } else {
        return speed(A,B,nodeWts[a]*nodeWts[b],edges[a][b]);
    }
}

vec2 speed_sq(vec2 p,float strength)
{
    float L = p.x*p.x + p.y*p.y;
    vec2 d = -normalize(p);
	
	vec2 hooke  =  d * H * log(L) * strength;
    
    return hooke;
}
